/*
 * Copyright (c) 2008, 2009, 2010, 2011, 2013, 2014 Nicira, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __NETLINK_H_
#define __NETLINK_H_ 1

#include "Types.h"
#include "NetlinkProto.h"

/* Netlink attribute types. */
typedef enum
{
    NL_A_NO_ATTR = 0,
    NL_A_UNSPEC,
    NL_A_U8,
    NL_A_U16,
    NL_A_BE16 = NL_A_U16,
    NL_A_U32,
    NL_A_BE32 = NL_A_U32,
    NL_A_U64,
    NL_A_BE64 = NL_A_U64,
    NL_A_STRING,
    NL_A_FLAG,
    NL_A_NESTED,
    N_NL_ATTR_TYPES
} NL_ATTR_TYPE;

/* Netlink attribute policy.
 * Specifies the policy for parsing for netlink attribute. */
typedef struct _NL_POLICY
{
    NL_ATTR_TYPE type;
    UINT32 minLen;
    UINT32 maxLen;
} NL_POLICY, *PNL_POLICY;

/* This macro is careful to check for attributes with bad lengths. */
#define NL_ATTR_FOR_EACH(ITER, LEFT, ATTRS, ATTRS_LEN)                  \
    for ((ITER) = (ATTRS), (LEFT) = (ATTRS_LEN);                        \
         NlAttrIsValid(ITER, LEFT);                                     \
         (LEFT) -= NlAttrLenPad(ITER, LEFT), (ITER) = NlAttrNext(ITER))

/* This macro does not check for attributes with bad lengths.  It should only
 * be used with messages from trusted sources or with messages that have
 * already been validated (e.g. with NL_ATTR_FOR_EACH).  */
#define NL_ATTR_FOR_EACH_UNSAFE(ITER, LEFT, ATTRS, ATTRS_LEN)           \
    for ((ITER) = (ATTRS), (LEFT) = (ATTRS_LEN);                        \
         (LEFT) > 0;                                                    \
         (LEFT) -= NLA_ALIGN((ITER)->nlaLen), (ITER) = NlAttrNext(ITER))

#define NL_ATTR_GET_AS(NLA, TYPE) \
        (*(TYPE*) NlAttrGetUnspec(nla, sizeof(TYPE)))

/* Netlink message accessing the payload */
PVOID NlMsgAt(const PNL_MSG_HDR nlh, UINT32 offset);
UINT32 NlMsgSize(const PNL_MSG_HDR nlh);
PCHAR NlMsgPayload(const PNL_MSG_HDR nlh);
UINT32 NlMsgPayloadLen(const PNL_MSG_HDR nlh);
PNL_ATTR NlMsgAttrs(const PNL_MSG_HDR nlh);
INT NlMsgAttrLen(const PNL_MSG_HDR nlh);

/* Netlink message parse */
PNL_MSG_HDR NlMsgNext(const PNL_MSG_HDR nlh);
INT NlAttrIsValid(const PNL_ATTR nla, UINT32 maxlen);
UINT32 NlAttrLenPad(const PNL_ATTR nla, UINT32 maxlen);

/* Netlink attribute parsing. */
UINT32 NlAttrMinLen(NL_ATTR_TYPE type);
UINT32 NlAttrMinLen(NL_ATTR_TYPE type);
PNL_ATTR NlAttrNext(const PNL_ATTR nla);
UINT16 NlAttrType(const PNL_ATTR nla);
PVOID NlAttrData(const PNL_ATTR nla);
UINT32 NlAttrGetSize(const PNL_ATTR nla);
const PVOID NlAttrGet(const PNL_ATTR nla);
const PVOID NlAttrGetUnspec(const PNL_ATTR nla, UINT32 size);
BE64 NlAttrGetBe64(const PNL_ATTR nla);
BE32 NlAttrGetBe32(const PNL_ATTR nla);
UINT8 NlAttrGetU8(const PNL_ATTR nla);
UINT32 NlAttrGetU32(const PNL_ATTR nla);
const PNL_ATTR NlAttrFind__(const PNL_ATTR attrs,
                            UINT32 size, UINT16 type);
const PNL_ATTR NlAttrFindNested(const PNL_ATTR nla,
                                UINT16 type);
BOOLEAN NlAttrParse(const PNL_MSG_HDR nlMsg, UINT32 attrOffset,
                    const NL_POLICY policy[],
                    PNL_ATTR attrs[], UINT32 n_attrs);

/* Netlink attribute validation */
BOOLEAN NlAttrValidate(const PNL_ATTR, const PNL_POLICY);

#endif /* __NETLINK_H_ */
