/*
 * Copyright (c) 2008, 2009, 2010, 2011, 2012, 2013, 2014 Nicira, Inc.
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

#include "precomp.h"
#include "NetlinkProto.h"
#include "Netlink.h"

#ifdef OVS_DBG_MOD
#undef OVS_DBG_MOD
#endif
#define OVS_DBG_MOD OVS_DBG_NETLINK
#include "Debug.h"

/*
 * ---------------------------------------------------------------------------
 * Netlink message accessing the payload.
 * ---------------------------------------------------------------------------
 */
PVOID
NlMsgAt(const PNL_MSG_HDR nlh, UINT32 offset)
{
    return ((PCHAR)nlh + offset);
}

/*
 * ---------------------------------------------------------------------------
 * Returns the size of netlink message.
 * ---------------------------------------------------------------------------
 */
UINT32
NlMsgSize(const PNL_MSG_HDR nlh)
{
    return nlh->nlmsgLen;
}

/*
 * ---------------------------------------------------------------------------
 * Returns pointer to nlmsg payload.
 * ---------------------------------------------------------------------------
 */
PCHAR
NlMsgPayload(const PNL_MSG_HDR nlh)
{
    return ((PCHAR)nlh + NLMSG_HDRLEN);
}

/*
 * ---------------------------------------------------------------------------
 * Returns length of nlmsg payload.
 * ---------------------------------------------------------------------------
 */
UINT32
NlMsgPayloadLen(const PNL_MSG_HDR nlh)
{
    return nlh->nlmsgLen - NLMSG_HDRLEN;
}

/*
 * ---------------------------------------------------------------------------
 * Returns pointer to nlmsg attributes.
 * ---------------------------------------------------------------------------
 */
PNL_ATTR
NlMsgAttrs(const PNL_MSG_HDR nlh)
{
    return (PNL_ATTR) (NlMsgPayload(nlh) + GENL_HDRLEN + OVS_HDRLEN);
}

/*
 * ---------------------------------------------------------------------------
 * Returns size of to nlmsg attributes.
 * ---------------------------------------------------------------------------
 */
INT
NlMsgAttrLen(const PNL_MSG_HDR nlh)
{
    return NlMsgPayloadLen(nlh) - GENL_HDRLEN - OVS_HDRLEN;
}

/* Netlink message parse. */

/*
 * ---------------------------------------------------------------------------
 * Returns next netlink message in the stream.
 * ---------------------------------------------------------------------------
 */
PNL_MSG_HDR
NlMsgNext(const PNL_MSG_HDR nlh)
{
    return (PNL_MSG_HDR)((PCHAR)nlh +
            NLMSG_ALIGN(nlh->nlmsgLen));
}

/*
 * ---------------------------------------------------------------------------
 * Netlink Attr helper APIs.
 * ---------------------------------------------------------------------------
 */
INT
NlAttrIsValid(const PNL_ATTR nla, UINT32 maxlen)
{
    return (maxlen >= sizeof *nla
            && nla->nlaLen >= sizeof *nla
            && nla->nlaLen <= maxlen);
}

/*
 * ---------------------------------------------------------------------------
 * Returns alligned length of the attribute.
 * ---------------------------------------------------------------------------
 */
UINT32
NlAttrLenPad(const PNL_ATTR nla, UINT32 maxlen)
{
    UINT32 len = NLA_ALIGN(nla->nlaLen);

    return len <= maxlen ? len : nla->nlaLen;
}

/*
 * ---------------------------------------------------------------------------
 * Default minimum payload size for each type of attribute.
 * ---------------------------------------------------------------------------
 */
UINT32
NlAttrMinLen(NL_ATTR_TYPE type)
{
    switch (type) {
    case NL_A_NO_ATTR: return 0;
    case NL_A_UNSPEC: return 0;
    case NL_A_U8: return 1;
    case NL_A_U16: return 2;
    case NL_A_U32: return 4;
    case NL_A_U64: return 8;
    case NL_A_STRING: return 1;
    case NL_A_FLAG: return 0;
    case NL_A_NESTED: return 0;
    case N_NL_ATTR_TYPES:
    default:
    OVS_LOG_WARN("Unsupprted attribute type: %d", type);
    ASSERT(0);
    }

    /* To keep compiler happy */
    return 0;
}

/*
 * ---------------------------------------------------------------------------
 * Default maximum payload size for each type of attribute.
 * ---------------------------------------------------------------------------
 */
UINT32
NlAttrMaxLen(NL_ATTR_TYPE type)
{
    switch (type) {
    case NL_A_NO_ATTR: return SIZE_MAX;
    case NL_A_UNSPEC: return SIZE_MAX;
    case NL_A_U8: return 1;
    case NL_A_U16: return 2;
    case NL_A_U32: return 4;
    case NL_A_U64: return 8;
    case NL_A_STRING: return SIZE_MAX;
    case NL_A_FLAG: return SIZE_MAX;
    case NL_A_NESTED: return SIZE_MAX;
    case N_NL_ATTR_TYPES:
    default:
    OVS_LOG_WARN("Unsupprted attribute type: %d", type);
    ASSERT(0);
    }

    /* To keep compiler happy */
    return 0;
}

/* Netlink attribute iteration. */

/*
 * ---------------------------------------------------------------------------
 * Returns the next attribute.
 * ---------------------------------------------------------------------------
 */
PNL_ATTR
NlAttrNext(const PNL_ATTR nla)
{
    return (PNL_ATTR)((UINT8 *)nla + NLA_ALIGN(nla->nlaLen));
}

/*
 * --------------------------------------------------------------------------
 * Returns the bits of 'nla->nlaType' that are significant for determining
 * its type.
 * --------------------------------------------------------------------------
 */
UINT16
NlAttrType(const PNL_ATTR nla)
{
   return nla->nlaType & NLA_TYPE_MASK;
}

/*
 * --------------------------------------------------------------------------
 * Returns the netlink attribute data.
 * --------------------------------------------------------------------------
 */
PVOID
NlAttrData(const PNL_ATTR nla)
{
    return ((PCHAR)nla + NLA_HDRLEN);
}

/*
 * ---------------------------------------------------------------------------
 * Returns the number of bytes in the payload of attribute 'nla'.
 * ---------------------------------------------------------------------------
 */
UINT32
NlAttrGetSize(const PNL_ATTR nla)
{
    return nla->nlaLen - NLA_HDRLEN;
}

/*
 * ---------------------------------------------------------------------------
 * Returns the first byte in the payload of attribute 'nla'.
 * ---------------------------------------------------------------------------
 */
const PVOID
NlAttrGet(const PNL_ATTR nla)
{
    ASSERT(nla->nlaLen >= NLA_HDRLEN);
    return nla + 1;
}

/*
 * ---------------------------------------------------------------------------
 * Asserts that 'nla''s payload is at least 'size' bytes long, and returns the
 * first byte of the payload.
 * ---------------------------------------------------------------------------
 */
const
PVOID NlAttrGetUnspec(const PNL_ATTR nla, UINT32 size)
{
    UNREFERENCED_PARAMETER(size);
    ASSERT(nla->nlaLen >= NLA_HDRLEN + size);
    return nla + 1;
}

/*
 * ---------------------------------------------------------------------------
 * Returns the 64-bit network byte order value in 'nla''s payload.
 *
 * Asserts that 'nla''s payload is at least 8 bytes long.
 * ---------------------------------------------------------------------------
 */
BE64
NlAttrGetBe64(const PNL_ATTR nla)
{
    return NL_ATTR_GET_AS(nla, BE64);
}

/*
 * ---------------------------------------------------------------------------
 * Returns the 32-bit network byte order value in 'nla''s payload.
 *
 * Asserts that 'nla''s payload is at least 4 bytes long.
 * ---------------------------------------------------------------------------
 */
BE32
NlAttrGetBe32(const PNL_ATTR nla)
{
    return NL_ATTR_GET_AS(nla, BE32);
}

/*
 * ---------------------------------------------------------------------------
 * Returns the 8-bit value in 'nla''s payload.
 * ---------------------------------------------------------------------------
 */
UINT8
NlAttrGetU8(const PNL_ATTR nla)
{
    return NL_ATTR_GET_AS(nla, UINT8);
}

/*
 * ---------------------------------------------------------------------------
 * Returns the 32-bit host byte order value in 'nla''s payload.
 * Asserts that 'nla''s payload is at least 4 bytes long.
 * ---------------------------------------------------------------------------
 */
UINT32
NlAttrGetU32(const PNL_ATTR nla)
{
    return NL_ATTR_GET_AS(nla, UINT32);
}

/*
 * ---------------------------------------------------------------------------
 * Validate the netlink attribute against the policy
 * ---------------------------------------------------------------------------
 */
BOOLEAN
NlAttrValidate(const PNL_ATTR nla, const PNL_POLICY policy)
{
    UINT32 minLen;
    UINT32 maxLen;
    UINT32 len;
    BOOLEAN ret = FALSE;

    if (policy->type == NL_A_NO_ATTR) {
        ret = TRUE;
        goto done;
    }

    /* Figure out min and max length. */
    minLen = policy->minLen;
    if (!minLen) {
        minLen = NlAttrMinLen(policy->type);
    }
    maxLen = policy->maxLen;
    if (!maxLen) {
        maxLen = NlAttrMaxLen(policy->type);
    }

    /* Verify length. */
    len = NlAttrGetSize(nla);
    if (len < minLen || len > maxLen) {
        OVS_LOG_WARN("Attribute: %p, len: %d, not in valid range, "
                     "min: %d, max: %d", nla, len, minLen, maxLen);
        goto done;
    }

    /* Strings must be null terminated and must not have embedded nulls. */
    if (policy->type == NL_A_STRING) {
        if (((PCHAR) nla)[nla->nlaLen - 1]) {
            OVS_LOG_WARN("Attributes %p lacks null at the end", nla);
            goto done;
        }

        if (memchr(nla + 1, '\0', len - 1) != NULL) {
            OVS_LOG_WARN("Attributes %p has bad length", nla);
            goto done;
        }
    }

done:
    return ret;
}

/*
 * ---------------------------------------------------------------------------
 * Returns an attribute of type 'type' from a series of
 * attributes.
 * ---------------------------------------------------------------------------
 */
const PNL_ATTR
NlAttrFind__(const PNL_ATTR attrs, UINT32 size, UINT16 type)
{
    PNL_ATTR iter = NULL;
    PNL_ATTR ret = NULL;
    UINT32 left;

    NL_ATTR_FOR_EACH (iter, left, attrs, size) {
        if (NlAttrType(iter) == type) {
            ret = iter;
            goto done;
        }
    }

done:
    return ret;
}

/*
 * ---------------------------------------------------------------------------
 * Returns the first Netlink attribute within 'nla' with the specified
 * 'type'.
 *
 * This function does not validate the attribute's length.
 * ---------------------------------------------------------------------------
 */
const PNL_ATTR
NlAttrFindNested(const PNL_ATTR nla, UINT16 type)
{
    return NlAttrFind__((const PNL_ATTR)(NlAttrGet(nla)),
                         NlAttrGetSize(nla), type);
}

/*
 *----------------------------------------------------------------------------
 * Parses the netlink message at a given offset (attrOffset)
 * as a series of attributes. A pointer to the attribute with type
 * 'type' is stored in attrs at index 'type'. policy is used to define the
 * attribute type validation parameters.
 * 'nla_offset' should be NLMSG_HDRLEN + GENL_HDRLEN + OVS_HEADER
 *
 * Returns NDIS_STATUS_SUCCESS normally.  Fails only if packet data cannot be accessed
 * (e.g. if Pkt_CopyBytesOut() returns an error).
 *----------------------------------------------------------------------------
 */
BOOLEAN NlAttrParse(const PNL_MSG_HDR nlMsg, UINT32 attrOffset,
                    const NL_POLICY policy[],
                    PNL_ATTR attrs[], UINT32 n_attrs)
{
    PNL_ATTR nla;
    UINT32 left;
    UINT32 iter;
    BOOLEAN ret = FALSE;

    memset(attrs, 0, n_attrs * sizeof *attrs);

    if ((NlMsgSize(nlMsg) < attrOffset) || (!(NlMsgAttrLen(nlMsg)))) {
        OVS_LOG_WARN("No attributes in nlMsg: %p at offset: %d",
                     nlMsg, attrOffset);
        goto done;
    }

    NL_ATTR_FOR_EACH (nla, left, NlMsgAt(nlMsg, attrOffset),
                      NlMsgSize(nlMsg) - attrOffset)
    {
        UINT16 type = NlAttrType(nla);
        if (type < n_attrs && policy[type].type != NL_A_NO_ATTR) {
            /* Typecasting to keep the compiler happy */
            const PNL_POLICY e = (const PNL_POLICY)(&policy[type]);
            if (!NlAttrValidate(nla, e)) {
                goto done;
            }

            if (attrs[type]) {
                OVS_LOG_WARN("Duplicate attribute in nlMsg: %p, "
                             "type: %u", nlMsg, type);
            }

            attrs[type] = nla;
        }
    }

    if (left) {
        OVS_LOG_ERROR("Attributes followed by garbage");
        goto done;
    }

    for (iter = 0; iter < n_attrs; iter++) {
        const PNL_POLICY e = (const PNL_POLICY)(&policy[iter]);
        if (e->type != NL_A_NO_ATTR && !attrs[iter]) {
            OVS_LOG_ERROR("Required attr:%d missing", iter);
            goto done;
        }
    }

    ret = TRUE;

done:
    return ret;
}
