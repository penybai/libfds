/**
 * \file   include/libfds/template.h
 * \author Lukas Hutak <lukas.hutak@cesnet.cz>
 * \brief  Template (header file)
 * \date   October 2017
 */
/*
 * Copyright (C) 2017 CESNET, z.s.p.o.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name of the Company nor the names of its contributors
 *    may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * ALTERNATIVELY, provided that this notice is retained in full, this
 * product may be distributed under the terms of the GNU General Public
 * License (GPL) version 2 or later, in which case the provisions
 * of the GPL apply INSTEAD OF those given above.
 *
 * This software is provided ``as is'', and any express or implied
 * warranties, including, but not limited to, the implied warranties of
 * merchantability and fitness for a particular purpose are disclaimed.
 * In no event shall the company or contributors be liable for any
 * direct, indirect, incidental, special, exemplary, or consequential
 * damages (including, but not limited to, procurement of substitute
 * goods or services; loss of use, data, or profits; or business
 * interruption) however caused and on any theory of liability, whether
 * in contract, strict liability, or tort (including negligence or
 * otherwise) arising in any way out of the use of this software, even
 * if advised of the possibility of such damage.
 *
 */

#ifndef LIBFDS_TEMPLATE_H
#define LIBFDS_TEMPLATE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "iemgr.h"
#include <libfds/api.h>

/**
 * \defgroup fds_template IPFIX (Options) Template
 * \ingroup publicAPIs
 * \brief Tools for processing of IPFIX (Options) Templates
 * \remark Based on RFC 7011, Section 3.4.1. - 3.4.2.
 *   (see https://tools.ietf.org/html/rfc7011#section-3)
 *
 * @{
 */

/** Unsigned integer type able to hold all template flags */
typedef uint16_t fds_template_flag_t;

/** \brief Template Field features                                                               */
enum fds_tfield_features {
    /**
     * \brief Scope field
     *
     * If this state flag is set, this is a scope field.
     */
    FDS_TFIELD_SCOPE = (1 << 0),
        /**
     * \brief Multiple occurrences of this Information Element (IE)
     *
     * If this flag is set, there are multiple occurrences of this IE anywhere in the template
     * to which the field belongs.
     */
    FDS_TFIELD_MULTI_IE = (1 << 1),
    /**
     * \brief The last occurrence of this Information Elements (IE)
     *
     * If this flags is set, there are NO more occurrences of the IE with the same combination of
     * an Information Element ID and an Enterprise Number in a template to which the field belongs.
     * In other words, if this flag is NOT set, there is at least one IE with the same definition
     * and with _higher_ index in the template.
     * \note This flag is also set if there are NOT multiple occurrences of the same IE.
     */
    FDS_TFIELD_LAST_IE = (1 << 2),
    /**
     * \brief Flow key Information Element
     *
     * \note To distinguish whether the IE is a flow key or not, an exporter must send a special
     *   record. In other words, this information is not a part of a template definition. See
     *   fds_template_define_flowkey() for more information.
     */
    FDS_TFIELD_FKEY = (1 << 3),
    /**
     * \brief Is it a field of structured data
     *
     * In other words, if this flag is set, the field is the basicList, subTemplateList, or
     * subTemplateMultiList Information Element (see RFC 6313).
     * \note To distinguish whether the IE is structured or not, an external database of IEs must
     *   be used. For example, use the IE manager distributed with libfds. In other words, this
     *   information is not a part of a template definition. See fds_template_define_ies() for
     *   more information.
     */
    FDS_TFIELD_STRUCT = (1 << 4),
    /**
     * \brief Reverse Information Element
     *
     * An Information Element defined as corresponding to a normal (or forward) Information
     * Element, but associated with the reverse direction of a Biflow.
     * \note To distinguish whether the IE is reverse or not, an external database of IEs must be
     *   used. For example, use the IE manager distributed with libfds. In other words, this
     *   information is not a part of a template definition. See fds_template_define_ies() for
     *   more information.
     */
    FDS_TFIELD_REVERSE = (1 << 5),
    /**
     * \brief Biflow Directional or Non-directional Key field (Common field)
     *
     * Represents a field common for both flow directions.
     */
    FDS_TFIELD_BKEY = (1 << 6)
};

/*
 * Note: Biflow and template field flags
 * How field flags are used? Flags (#FDS_TFIELD_REVERSE, #FDS_TFIELD_BKEY) are set this way:
 *   - Directional and Non-directional Key fields: #FDS_TFIELD_BKEY
 *   - Forward only fields: [none of them]
 *   - Reverse only fields: #FDS_TFIELD_REVERSE
 * For example:
 *
 * +--------+----------+- ------+----------+-------+-- ---+-------+----------+-----------+
 * | src IP | src port | dst IP | dst port | proto | Pkts | Bytes | Pkts_Rev | Bytes_Rev |
 * +--------+----------+--------+----------+-------+------+-------+----------+-----------+
 *  \______________________  ___________________ _/ \______  ____/ \_________  _________/
 *                         \/                              \/                \/
 *                   FDS_TFIELD_BKEY                   [no flags]    FDS_TFIELD_REVERSE
 */

/** \brief Structure of a parsed IPFIX element in an IPFIX template                              */
struct fds_tfield {
    /** Enterprise Number      */
    uint32_t en;
    /** Information Element ID */
    uint16_t id;
    /**
     * \brief The real length of the Information Element.
     *
     * The value #FDS_IPFIX_VAR_IE_LEN (i.e. 65535) is reserved for variable-length information
     *   elements.
     */
    uint16_t length;
    /**
     * \brief The offset from the start of a data record in octets.
     *
     * The value #FDS_IPFIX_VAR_IE_LEN (i.e. 65535) is reserved for unknown offset if there is
     * at least one variable-length element among preceding elements in the same template.
     */
    uint16_t offset;
    /**
     * \brief Features specific for this field.
     *
     * The flags argument contains a bitwise OR of zero or more of the flags defined in
     * #fds_tfield_features enumeration.
     */
    fds_template_flag_t flags;
    /**
     * \brief Detailed definition of the element (data/semantic/unit type).
     *
     * If the definition is missing in the configuration, the values is NULL.
     */
    const struct fds_iemgr_elem *def;
};

/** Types of IPFIX (Options) Templates                                                           */
enum fds_template_type {
    FDS_TYPE_TEMPLATE,       /**< Definition of Template                                         */
    FDS_TYPE_TEMPLATE_OPTS,  /**< Definition of Options Template                                 */
    FDS_TYPE_TEMPLATE_UNDEF  /**< Type is not defined                                            */
};

/**
 * \brief Types of Options Templates
 *
 * These types of Option Templates are automatically recognized by template parser. Keep in mind
 * that multiple types can be detected at the same time.
 * \remark Standard types comes are based on RFC 7011, Section 4.
 */
enum fds_template_opts_type {
    /** The Metering Process Statistics Options Template                                         */
    FDS_OPTS_MPROC_STAT = (1 << 0),
    /** The Metering Process Reliability Statistics Options Template                             */
    FDS_OPTS_MPROC_RELIABILITY_STAT = (1 << 1),
    /** The Exporting Process Reliability Statistics Options Template                            */
    FDS_OPTS_EPROC_RELIABILITY_STAT = (1 << 2),
    /** The Flow Keys Options Template                                                           */
    FDS_OPTS_FKEYS = (1 << 3),
    /** The Information Element Type Options Template (RFC 5610)                                 */
    FDS_OPTS_IE_TYPE = (1 << 4)
    // Add new ones here...
};

/** \brief Template features                                                                     */
enum fds_template_features {
    /** Template has multiple occurrences of the same Information Element (IE) in the template.  */
    FDS_TEMPLATE_MULTI_IE = (1 << 0),
    /** Template has at least one Information Element of variable length                         */
    FDS_TEMPLATE_DYNAMIC = (1 << 1),
    /** Template described a Biflow record (it has at least one Reverse Information Element)     */
    FDS_TEMPLATE_BIFLOW = (1 << 2),
    /** Template has at least one structured data type (basicList, subTemplateList, etc.)        */
    FDS_TEMPLATE_STRUCT = (1 << 3),
    /** Template has a known flow key (at least one field is marked as a Flow Key)               */
    FDS_TEMPLATE_FKEY = (1 << 4),
    // Add new ones here...
};

/**
 * \brief Structure for a parsed IPFIX template
 *
 * This dynamically sized structure wraps a parsed copy of an IPFIX template.
 * \warning Never modify values directly. Otherwise, consistency of the template cannot be
 *   guaranteed!
 */
struct fds_template {
    /** Type of the template                                                                     */
    enum fds_template_type type;
    /**
     * \brief Type of the Option Template
     * \note Valid only when #type == ::FDS_TYPE_TEMPLATE_OPTS
     * \see #fds_template_opts_type
     */
    uint32_t opts_types;

    /** Template ID                                                                              */
    uint16_t id;
    /**
     * \brief Features specific for this template.
     * The flags argument contains a bitwise OR of zero or more of the flags defined in
     * #fds_template_features enumeration.
     */
    fds_template_flag_t flags;

    /**
     * \brief Length of a data record using this template.
     * \warning If the template has at least one Information Element of variable-length encoding,
     *   i.e. #flags \& ::FDS_TEMPLATE_DYNAMIC is true, this value represents
     *   the smallest possible length of corresponding data record. Otherwise represents real
     *   length of the data record.
     */
    uint16_t data_length;

    /** Raw binary copy of the template (starts with a header)                                   */
    struct raw_s {
        /** Length of the record (in bytes)                                                      */
        uint16_t length;
        /** Pointer to the copy of template record (starts with a header)                        */
        uint8_t *data;
    } raw; /**< Raw template record                                                              */

    /**
     * \brief Time information related to Exporting Process
     * \warning All timestamps (seconds since UNIX epoch) are based on "Export Time" from the
     *   IPFIX message header.
     */
    struct time_s {
        /** The first reception                                                                  */
        uint32_t first_seen;
        /** The last reception (a.k.a. refresh time)                                             */
        uint32_t last_seen;
        /** End of life (the time after which the template is not valid anymore) (UDP only).
         * If the value is different than last_seen, the template has defined timeout.
         */
        uint32_t end_of_life;
    } time; /**< Instance of the structure                                                       */

    /**
     * Total number of fields
     * If the value is zero, this template is so-called Template Withdrawal.
     */
    uint16_t fields_cnt_total;
    /** Number of scope fields (first N records of the Options Template)                         */
    uint16_t fields_cnt_scope;

    /**
     * \brief Array of parse fields (reverse view)
     *
     * The array is defined only if the template describes a Biflow record. In other words, at
     * least one template field is defined as reverse. Information whether a field is reverse or
     * not is not part of IPFIX template definition and must be determined based on external
     * knowledge. This array can be generated by fds_template_ies_define() function.
     *
     * Directional Key fields are converted to opposite direction fields (for example, source IPv4
     * to destination IPv4 and vice versa). Non-directional Key fields are untouched (for example,
     * protocol). Forward only fields are converted to reverse only fields and vice versa.
     * (for example, bytes to bytesReverse). See the example below:
     * \verbatim
     *  Forward template fields (fds_template#fields)
     *  +--------+----------+--------+----------+-------+---------+----------+---------+----------+
     *  | src IP | src port | dst IP | dst port | proto |   Pkts  |   Bytes  | PktsRev | BytesRev |
     *  +--------+----------+--------+----------+-------+---------+----------+---------+----------+
     *      |          |         |         |        |        |          |         |         |
     *      V          V         V         V        V        V          V         V         V
     *  +--------+----------+--------+----------+-------+---------+----------+---------+----------+
     *  | dst IP | dst port | src IP | src port | proto | PktsRev | BytesRev |   Pkts  |   Bytes  |
     *  +--------+----------+--------+----------+-------+---------+----------+---------+----------+
     *  Reverse template fields (fds_template#fields_rev)
     * \endverbatim
     * Compared to forward template array, this template fields of this reverse array can have
     * different PEN, ID, flags and reference to a definition of an IE.
     *
     * \warning Biflow templates only!
     */
    struct fds_tfield *fields_rev;

    /**
     * Array of parsed fields.
     * This element MUST be the last element in this structure.
     */
    struct fds_tfield fields[1];
};

/**
 * \brief Parse an IPFIX template
 *
 * Try to parse the template from a memory pointed by \p ptr of at most \p len bytes. Typically
 * during processing of a (Options) Template Set, the parameter \p len is length to the end of
 * the (Options) Template Set. After successful parsing, the function will set the value to
 * the real length of the raw template (in octets). The \p len can be, therefore, used to jump to
 * the beginning of the next template definition.
 *
 * Some information of the template structure after parsing the template are still unknown.
 *   - All timestamps (fds_template#time). Default values are zeros.
 *   - Only following template field flags (i.e. fds_tfield#flags) could be set: ::FDS_TFIELD_SCOPE,
 *     ::FDS_TFIELD_MULTI_IE and ::FDS_TFIELD_LAST_IE. Others are cleared, by default.
 *   - Only following global template flags (i.e. fds_template#flags) could be set:
 *     ::FDS_TEMPLATE_MULTI_IE and ::FDS_TEMPLATE_DYNAMIC. Others are cleared, by default.
 *   - Reverse template fields are not defined.
 *
 * Other structure's members are usually filled and managed by a template manager (::fds_tmgr_t)
 * to which the template is inserted.
 *
 * \param[in]     type  Type of template (::FDS_TYPE_TEMPLATE or ::FDS_TYPE_TEMPLATE_OPTS)
 * \param[in]     ptr   Pointer to the header of the template
 * \param[in,out] len   [in] Maximal length of the raw template /
 *                      [out] real length of the raw template in  octets (see notes)
 * \param[out]    tmplt Parsed template (automatically allocated)
 * \return On success, the function will set parameters \p tmplt, \p len and return #FDS_OK.
 *   Otherwise, the parameters will be unchanged and the function will return #FDS_ERR_FORMAT or
 *   #FDS_ERR_NOMEM.
 */
FDS_API int
fds_template_parse(enum fds_template_type type, const void *ptr, uint16_t *len,
    struct fds_template **tmplt);

/**
 * \brief Create a copy of a template structure
 *
 * \warning Keep in mind that references to the definitions of template fields will be preserved.
 *   If you do not have control over a corresponding Information Element manager, you should
 *   remove the references using the fds_template_define_ies() function.
 * \param[in] tmplt Original template
 * \return Pointer to the copy or NULL (in case of memory allocation error).
 */
FDS_API struct fds_template *
fds_template_copy(const struct fds_template *tmplt);

/**
 * \brief Destroy a template
 * \param[in] tmplt Template instance
 */
FDS_API void
fds_template_destroy(struct fds_template *tmplt);

/**
 * \brief Compare templates (only based on template fields)
 *
 * Only raw templates are compared i.e. everything is ignored except Template ID
 * and template fields (Information Element ID, Private Enterprise Number and length)
 * \param t1 First template
 * \param t2 Second template
 * \return The function returns an integer less than, equal to, or greater than zero if the first
 *   template is found, respectively, to be less than, to match, or be greater than the second
 *   template.
 */
FDS_API int
fds_template_cmp(const struct fds_template *t1, const struct fds_template *t2);

/**
 * \brief Find the first occurrence of an Information Element in a template
 * \param[in] tmplt Template structure
 * \param[in] en    Enterprise Number
 * \param[in] id    Information Element ID
 * \return Pointer to the IE or NULL.
 */
FDS_API struct fds_tfield *
fds_template_find(struct fds_template *tmplt, uint32_t en, uint16_t id);

/**
 * \copydoc fds_template_find()
 */
FDS_API const struct fds_tfield *
fds_template_cfind(const struct fds_template *tmplt, uint32_t en, uint16_t id);

/**
 * \brief Add references to Information Element definitions and update corresponding flags
 *
 * The function will try to find a definition of each template field in a manager of IE definitions
 * based on Information Element ID and Private Enterprise Number of the template field.
 * Template flags (i.e. ::FDS_TEMPLATE_BIFLOW and ::FDS_TEMPLATE_STRUCT) and
 * field flags (i.e. ::FDS_TFIELD_STRUCT, ::FDS_TFIELD_REVERSE, ::FDS_TFIELD_BKEY) that can be
 * determined from the definitions will be set appropriately.
 *
 * If the template corresponds to a Biflow record, this function will also define reverse template
 * fields (fds_template#fields_rev) and perform necessary modifications.
 *
 * \note If the manager is _not defined_ (i.e. NULL) and preserve mode is _disabled_, all the
 *   template field references to the definitions will be removed and corresponding flags will be
 *   cleared.
 * \note If the manager is _defined_ and preserve mode is _disabled_, all the template field
 *   references to the definitions will be updated. If any field doesn't have a corresponding
 *   definition in the user defined manager, the old reference will be removed.
 * \note If the manager is _defined_ and preserve mode is _enabled_, the function will update only
 *   template fields without the known references. This allows you to use multiple definition
 *   managers (for example, primary and secondary) at the same time. If the template wasn't
 *   earlier defined as a Biflow template (flag ::FDS_TEMPLATE_BIFLOW is not set), all reverse
 *   fields will be ignored.
 * \note If the manager is _not_defined_ and preserve mode is _enabled_, the function does nothing.
 *
 * \param[in] tmplt    Template structure
 * \param[in] iemgr    Manager of Information Elements definitions (can be NULL)
 * \param[in] preserve If any field already has a reference to a definition, do not replace it with
 *   a new definition. In other words, add definitions only to undefined fields.
 * \return #FDS_OK on success.
 * \return #FDS_ERR_NOMEM on memory allocation error. The template is not consistent and should be
 *   freed or the function should be called again without enabled preserve mode.
 */
FDS_API int
fds_template_ies_define(struct fds_template *tmplt, const fds_iemgr_t *iemgr, bool preserve);

/**
 * \brief Check if a flow key is applicable
 *
 * \param[in] tmplt   Template structure
 * \param[in] flowkey Flow key
 * \return On success returns #FDS_OK.
 *   If the \p flowkey tries to set non-existing template fields as flow key, the function will
 *   return #FDS_ERR_FORMAT and no modifications will be performed.
 */
FDS_API int
fds_template_flowkey_applicable(const struct fds_template *tmplt, uint64_t flowkey);

/**
 * \brief Add a flow key
 *
 * Flow key is a set of bit fields that is used for marking the Information Elements of a Data
 * Record that serve as Flow Key. Each bit represents an Information Element in the Data Record,
 * with the n-th least significant bit representing the n-th Information Element. A bit set to
 * value 1 indicates that the corresponding Information Element is a Flow Key of the reported
 * Flow. A bit set to value 0 indicates that this is not the case. For more information, see
 * RFC 7011, Section 4.4
 *
 * The function will set flow key flag (i.e.::FDS_TFIELD_FKEY) to corresponding template
 * fields and the global template flag ::FDS_TEMPLATE_FKEY.
 * \note If the \p flowkey parameter is zero, the flags will be clear from the template and the
 *   fields.
 *
 * \param[in] tmplt   Template structure
 * \param[in] flowkey Flow key
 * \return On success, the function will return #FDS_OK.
 *   If the \p flowkey tries to set non-existing template fields as flow keys, the function will
 *   return #FDS_ERR_FORMAT and no modification will be performed.
 */
FDS_API int
fds_template_flowkey_define(struct fds_template *tmplt, uint64_t flowkey);

/**
 * \brief Compare a flow key
 *
 * Check if the flow key of a template is the same as \p flowkey.
 * For more information about the flow key see: fds_template_flowkey_define().
 * \param[in] tmplt   Template structure
 * \param[in] flowkey Flow key
 * \return If the keys are the same returns 0. Otherwise returns a non-zero value.
 */
FDS_API int
fds_template_flowkey_cmp(const struct fds_template *tmplt, uint64_t flowkey);

#ifdef __cplusplus
}
#endif
#endif // LIBFDS_TEMPLATE_H

/**
 * @}
 */
