/*
 * Copyright (C) 1990 by
 * Digital Equipment Corporation, Maynard, Massachusetts.
 * All rights reserved.
 *
 * This software is furnished under a license and may be used and copied
 * only  in  accordance  of  the  terms  of  such  license  and with the
 * inclusion of the above copyright notice. This software or  any  other
 * copies thereof may not be provided or otherwise made available to any
 * other person.  No title to and  ownership of the  software is  hereby
 * transferred.
 *
 * The information in this software is  subject to change without notice
 * and  should  not  be  construed  as a commitment by Digital Equipment
 * Corporation.
 *
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by Digital.
 *
 *
 * Abstract:	Small Computer System Interface (SCSI) Definitions
 *
 * Author:	Stephen F. Shirron
 *
 * Modifications:
 *
 *	sfs	30-May-1990	Initial entry.
 */
 
#define scsi_k_cmd_test_unit_ready 0
#define scsi_k_cmd_rewind 1
#define scsi_k_cmd_request_sense 3
#define scsi_k_cmd_mk_read 8 
#define scsi_k_cmd_mk_write 10
#define scsi_k_cmd_mk_write_filemarks 16
#define scsi_k_cmd_inquiry 18
#define scsi_k_cmd_mode_select 21
#define scsi_k_cmd_mode_sense 26         /* 0x1a */
#define scsi_k_cmd_start_unit 27
#define scsi_k_cmd_read_capacity 37
#define scsi_k_cmd_dk_read 40
#define scsi_k_cmd_dk_write 42
#define scsi_k_cmd_prin 94    /* 0x5e */
#define scsi_k_cmd_prout 95    /* 0x5f */
#define scsi_k_cmd_report_luns 160       /* 0xa0 */
#define scsi_k_cmd_wr_devid 235
#define scsi_k_cmd_rd_devid 236

#define scsi_k_msg_cmd_complete 0
#define scsi_k_msg_ext_message 1
#define scsi_k_msg_save_dp 2
#define scsi_k_msg_restore_dp 3
#define scsi_k_msg_disconnect 4
#define scsi_k_msg_reject 7
#define scsi_k_msg_identify 128

#define scsi_k_msg_id_lun 0
#define scsi_k_msg_id_disc 64

#define scsi_k_msg_ext_modify_dp 0
#define scsi_k_msg_ext_sdtr 1

#define scsi_k_sts_good 0
#define scsi_k_sts_check 2
#define scsi_k_sts_busy 8
#define scsi_k_sts_reservation_conflict 24 /* 0x18 */

#define scsi_k_inq_not_evpd 0
#define scsi_k_inq_evpd     1

#define scsi_k_pr_action_register 0        /* persistent reserve register  */
#define scsi_k_pr_scope_lu        0        /* p.r. applies to logical unit */
#define scsi_k_pr_type_shared     4        /* read share + write share     */
#define scsi_k_pr_parm_len       24        /* parameter list length        */
#define PERSISTENT_RESERVE_CONSOLE_KEY 1   /* console key used registering */


#define GET_LUN    1
#define SET_LUN    2
#define CLEAR_LUN  3
#define ZERO_MAP   4

#define MAX_UDID_VALUE 32767          /* VMS limit */


/*****************************************************************************

                Table 19 - Standard INQUIRY data format
+=====-========-========-========-========-========-========-========-========+
|  Bit|   7    |   6    |   5    |   4    |   3    |   2    |   1    |   0    |
|Byte |        |        |        |        |        |        |        |        |
|=====+==========================+============================================|
| 0   | Peripheral qualifier     |           Peripheral device type           |
|-----+-----------------------------------------------------------------------|
| 1   |  RMB   |                  Device-type modifier                        |
|-----+-----------------------------------------------------------------------|
| 2   |   ISO version   |       ECMA version       |  ANSI-approved version   |
|-----+-----------------+-----------------------------------------------------|
| 3   |  AERC  | TrmTsk | NormACA|Reserved|         Response data format      |
|-----+-----------------------------------------------------------------------|
| 4   |                           Additional length (n-4)                     |
|-----+-----------------------------------------------------------------------|
| 5   |                           Reserved                                    |
|-----+-----------------------------------------------------------------------|
| 6   |    Reserved     |VendSpec| MultiP | MChngr |     Reserved to SIP      |
|-----+-----------------------------------+--------+--------------------------|
| 7   | RelAdr |     Reserved to SIP      | Linked |TranDis | CmdQue | RsvSIP |
|-----+-----------------------------------------------------------------------|
| 8   | (MSB)                                                                 |
|- - -+- -                        Vendor identification                    - -|
| 15  |                                                                 (LSB) |
|-----+-----------------------------------------------------------------------|
| 16  | (MSB)                                                                 |
|- - -+- -                        Product identification                   - -|
| 31  |                                                                 (LSB) |
|-----+-----------------------------------------------------------------------|
| 32  | (MSB)                                                                 |
|- - -+- -                        Product revision level                   - -|
| 35  |                                                                 (LSB) |
|-----+-----------------------------------------------------------------------|
| 36  |                                                                       |
|- - -+- -                        Vendor-specific                          - -|
| 55  |                                                                       |
|-----+-----------------------------------------------------------------------|
| 56  |                                                                       |
|- - -+- -                        Reserved                                 - -|
| 95  |                                                                       |
|=====+=======================================================================|
|     |                       Vendor-specific parameters                      |
|=====+=======================================================================|
| 96  |                                                                       |
|- - -+- -                        Vendor-specific                          - -|
| n   |                                                                       |
+=============================================================================+

*****************************************************************************/

struct inquiry_standard {
    unsigned int    devtype     :5;
    unsigned int    periqual    :3;
    unsigned int    devmod      :6;
    unsigned int    RMB         :1;
    unsigned int    ANSIversion :3;
    unsigned int    ECMAversion :3;
    unsigned int    ISOversion  :2;
    unsigned int    rspdatafmt  :4;
    unsigned int    rsvd0       :1;
    unsigned int    normACA     :1;
    unsigned int    TrmTsk      :1;
    unsigned int    AERC        :1;
    unsigned int    addlen      :8;
    unsigned int    rsvd1       :8;
    unsigned int    rsvdSIP     :3;
    unsigned int    MChngr	:1;
    unsigned int    MultiP	:1;
    unsigned int    VendSpec	:1;
    unsigned int    rsvd2	:2;
    unsigned int    RsvSip	:1;
    unsigned int    CmdQue	:1;
    unsigned int    TranDis	:1;
    unsigned int    Linked	:1;
    unsigned int    RsvdSIP2	:3;
    unsigned int    RelAdr	:1;
    unsigned char   vendor_id   [8];
    unsigned char   product_id  [16];
    unsigned char   product_rev [4];
    unsigned char   vendor_spec [20];
    
    /*
     *  This is the extended inquiry data as provided by the Digital
     *  products from Colorodo (HSZ70, HSZ80, HSG80).
     */

    unsigned char   vendor_96_128[33];
    unsigned char   dummy_byte;         /* to make up for vendor_96_128's odd byte */
    unsigned char   node_id[8];         /* Node ID */
    unsigned char   vendor_138_241[104];/* to fill in bytes 138-241 */
    unsigned long   allocation_class;   /* Allocation Class */
    unsigned char   target;             /* current target address */
    unsigned char   lun;                /* current lun address */
};






/*****************************************************************************

8.4.5 Supported vital product data pages

The supported vital product data pages are shown in table 114.

            Table 114 - Supported vital product data pages
+=====-========-========-========-========-========-========-========-========+
|  Bit|   7    |   6    |   5    |   4    |   3    |   2    |   1    |   0    |
|Byte |        |        |        |        |        |        |        |        |
|=====+==========================+============================================|
| 0   | Peripheral qualifier     |           Peripheral device type           |
|-----+-----------------------------------------------------------------------|
| 1   |                           Page code (00h)                             |
|-----+-----------------------------------------------------------------------|
| 2   |                           Reserved                                    |
|-----+-----------------------------------------------------------------------|
| 3   |                           Page length (n-3)                           |
|-----+-----------------------------------------------------------------------|
| 4   |                                                                       |
|- - -+- -                        Supported page list                      - -|
| n   |                                                                       |
+=============================================================================+

The peripheral qualifier field and the peripheral device type field are
defined in 7.5.1.

The page code field shall be set to the value of the page code field in
the INQUIRY command descriptor block (see 7.5).

The page length field specifies the length of the supported page list. 
If the allocation length is too small to transfer all of the page, the
page length shall not be adjusted to reflect the truncation.

The supported page list field shall contain a list of all vital product
data page codes implemented for the target or logical unit in ascending
order beginning with page code 00h.
*****************************************************************************/

struct inquiry_p0 {
    unsigned int    devtype     :5;
    unsigned int    periqual    :3;
    unsigned char   page_code;
    unsigned char   rsvd;
    unsigned char   page_len;
    unsigned char   page_list[];
};




/****************************************************************************

8.4.6 Unit serial number page

This page (see table 115) provides a product serial number for the
target or logical unit.

                  Table 115 - Unit serial number page
+=====-========-========-========-========-========-========-========-========+
|  Bit|   7    |   6    |   5    |   4    |   3    |   2    |   1    |   0    |
|Byte |        |        |        |        |        |        |        |        |
|=====+==========================+============================================|
| 0   | Peripheral qualifier     |           Peripheral device type           |
|-----+-----------------------------------------------------------------------|
| 1   |                           Page code (80h)                             |
|-----+-----------------------------------------------------------------------|
| 2   |                           Reserved                                    |
|-----+-----------------------------------------------------------------------|
| 3   |                           Page length (n-3)                           |
|-----+-----------------------------------------------------------------------|
| 4   |                                                                       |
|- - -+- -                        Product serial number                    - -|
| n   |                                                                       |
+=============================================================================+

The peripheral qualifier field and the peripheral device type field are
defined in 7.5.1.

The page length field specifies the length of the product serial number. 
If the allocation length is too small to transfer all of the page, the
page length shall not be adjusted to reflect the truncation.

The product serial number field contains ASCII data that is vendor-
specific. The least significant ASCII character of the serial number
shall appear as the last byte of a successful data transfer.  If the
product serial number is not available, the target shall return ASCII
spaces (20h) in this field. 

****************************************************************************/

struct inquiry_p80 {
    unsigned char    type_qual;
    unsigned char    page_code;
    unsigned char    rsvd;  
    unsigned char    page_len;
    unsigned char    serial_number[];
};



/*****************************************************************************

7.21 REPORT LUNS command

The REPORT LUNS command (see table 56) requests that the logical unit
numbers of known peripheral logical units at the target be sent to the
application client.  The REPORT LUNS command shall return information
about only those logical units to which commands can be sent.

                    Table 56 - REPORT LUNS command
+=====-========-========-========-========-========-========-========-========+
|  Bit|   7    |   6    |   5    |   4    |   3    |   2    |   1    |   0    |
|Byte |        |        |        |        |        |        |        |        |
|=====+=======================================================================|
| 0   |                           Operation code (A0h)                        |
|-----+-----------------------------------------------------------------------|
| 1   |                                                                       |
|- - -+- -                        Reserved                                 - -|
| 5   |                                                                       |
|-----+-----------------------------------------------------------------------|
| 6   | (MSB)                                                                 |
|- - -+- -                        Allocation length                        - -|
| 9   |                                                                 (LSB) |
|-----+-----------------------------------------------------------------------|
| 10  |                           Reserved                                    |
|-----+-----------------------------------------------------------------------|
| 11  |                           Control                                     |
+=============================================================================+

The Allocation length shall be at least 16 bytes.  If the Allocation
length is less than 16 bytes, the device server shall return CHECK
CONDITION status.  The sense key shall be set to ILLEGAL REQUEST and the
additional sense data shall be set to INVALID FIELD IN CDB.

The Allocation length may not be sufficient to contain the logical unit
number values for all configured logical units.  This shall not be
considered an error.  The device server shall report as many logical
unit number values as will fit in the specified Allocation length.

The device server shall report the logical unit numbers of configured
logical units using the format shown in table 57.

            Table 57 - LUN reporting parameter list format
+=====-========-========-========-========-========-========-========-========+
|  Bit|   7    |   6    |   5    |   4    |   3    |   2    |   1    |   0    |
|Byte |        |        |        |        |        |        |        |        |
|=====+=======================================================================|
| 0   | (MSB)                                                                 |
|- - -+- -                        LUN list length (n-7)                    - -|
| 3   |                                                                 (LSB) |
|-----+-----------------------------------------------------------------------|
| 4   |                                                                       |
|- - -+- -                        Reserved                                 - -|
| 7   |                                                                       |
|=====+=======================================================================|
|     |                           LUN list                                    |
|=====+=======================================================================|
| 8   | (MSB)                                                                 |
|- - -+- -                        LUN                                      - -|
| 15  |                                                                 (LSB) |
|-----+-----------------------------------------------------------------------|
|     |                           .                                           |
|     |                           .                                           |
|     |                           .                                           |
|-----+-----------------------------------------------------------------------|
| n-7 | (MSB)                                                                 |
|- - -+- -                        LUN                                      - -|
| n   |                                                                 (LSB) |
+=============================================================================+

The LUN list length shall contain the length in bytes of the LUN list
that is available to be transferred.  The LUN list length is the number
of logical unit numbers reported multiplied by eight.  If the allocation
length in the command descriptor block is too small to transfer
information about all configured logical units, the LUN list length
value shall not be adjusted to reflect the truncation.

******************************************************************************/


/****************************************************************************


			P83 Definitions
			---------------

8.4.3 Device identification page

The optional device identification page (see table 108) provides the
means to retrieve zero or more identification descriptors applying to
the logical unit.  Logical units may have more than one identification
descriptor (e.g., if several types of identifier are supported).

Device identifiers shall be assigned to the peripheral device (e.g., a
disk drive) and not to the currently mounted media, in the case of
removable media devices.  Media identification is outside the scope of
this standard.  It is expected that operating systems will make use of
the device identifiers during system configuration activities to
determine whether alternate paths exist for the same peripheral device.

  NOTE 58 In the case of virtual logical units (e.g., volume sets as
  defined by the SCC), the device identification may be constructed in
  a vendor-specific manner.  Vendors should ensure that such device
  identifiers are globally unique.

                Table 108 - Device identification page
+=====-========-========-========-========-========-========-========-========+
|  Bit|   7    |   6    |   5    |   4    |   3    |   2    |   1    |   0    |
|Byte |        |        |        |        |        |        |        |        |
|=====+==========================+============================================|
| 0   | Peripheral qualifier     |           Peripheral device type           |
|-----+-----------------------------------------------------------------------|
| 1   |                           Page code (83h)                             |
|-----+-----------------------------------------------------------------------|
| 2   |                           Reserved                                    |
|-----+-----------------------------------------------------------------------|
| 3   |                           Page length (n-3)                           |
|=====+=======================================================================|
|     |                           Identification descriptor list              |
|=====+=======================================================================|
| 4   | (MSB)                                                                 |
|- - -+- -                        Identification descriptor (0)            - -|
|     |                           (see table 109)                       (LSB) |
|-----+-----------------------------------------------------------------------|
|     |                                   .     .     .                       |
|-----+-----------------------------------------------------------------------|
|     | (MSB)                                                                 |
|- - -+- -                        Identification descriptor (last)         - -|
| n   |                           (see table 109)                       (LSB) |
+=============================================================================+

                 Table 109 - Identification descriptor
+=====-========-========-========-========-========-========-========-========+
|  Bit|   7    |   6    |   5    |   4    |   3    |   2    |   1    |   0    |
|Byte |        |        |        |        |        |        |        |        |
|=====+===================================+===================================|
| 0   |           Reserved                |         Code set                  |
|-----+-----------------------------------+-----------------------------------|
| 1   |           Reserved                |         Identifier type           |
|-----+-----------------------------------------------------------------------|
| 2   |                           Reserved                                    |
|-----+-----------------------------------------------------------------------|
| 3   |                           Page length (n-3)                           |
|-----+-----------------------------------------------------------------------|
| 4   | (MSB)                                                                 |
|- - -+- -                        Identifier                               - -|
| n   |                                                                 (LSB) |
+=============================================================================+

Each Identification descriptor (see table 109) contains information
identifying the logical unit.  If the logical unit is accessible through
any other path, it shall return the same identification.

The Code set field specifies the code set used for the identifier field,
as described in table 110.  This field is intended to be an aid to
software that displays the identifier field.

                         Table 110 - Code set
+=======-==============================================================+
| Value | Description                                                  |
|=======+==============================================================|
|   0   | Reserved                                                     |
|-------+--------------------------------------------------------------|
|   1   | The identifier field contains binary values.                 |
|-------+--------------------------------------------------------------|
|   2   | The identifier field only contains ASCII graphic codes       |
|       | (i.e., code values 20h through 7Eh)                          |
|-------+--------------------------------------------------------------|
| 3-Fh  | Reserved                                                     |
+======================================================================+

The Identifier type field specifies the format and assignment authority
for the identifier, as described in table 111.

                      Table 111 - Identifier type
+=======-==============================================================+
| Value | Description                                                  |
|=======+==============================================================|
|   0   | No assignment authority was used and consequently the        |
|       | identifier may not be globally unique (i.e., the identifier  |
|       | is vendor specific)                                          |
|-------+--------------------------------------------------------------|
|   1   | The first 8 bytes of the identifier field are a Vendor ID    |
|       | (see annex C).  The organization associated with the Vendor  |
|       | ID is responsible for ensuring that the remainder of the     |
|       | identifier field is unique.  One recommended method of       |
|       | constructing the remainder of the identifier field is        |
|       | to concatenate the product identification field from the     |
|       | standard INQUIRY data field and the product serial number    |
|       | field from the unit serial number page.                      |
|-------+--------------------------------------------------------------|
|   2   | The identifier field contains an IEEE Extended Unique        |
|       | Identifier, 64-bit (EUI-64).  In this case, the identifier   |
|       | length field shall be set to 8.  Note that the IEEE          |
|       | guidelines for EUI-64 specify a method for unambiguously     |
|       | encapsulating an IEEE 48-bit identifier within an EUI-64.    |
|-------+--------------------------------------------------------------|
|   3   | The identifier field contains a FC-PH 64-bit Name_Identifier |
|       | field as defined in X3.230-1994.  In this case, the          |
|       | identifier length shall be set to 8.                         |
|-------+--------------------------------------------------------------|
| 4-Fh  | Reserved                                                     |
+======================================================================+

The identifier length field specifies the length in bytes of the
identifier.  If the allocation length field of the command descriptor
block is too small to transfer all of the identifier, the identifier
length shall not be adjusted to reflect the truncation.

The identifier field contains the identifier as described by the
identifier type, code set, and identifier length fields.

The example described in this paragraph and shown in table 112 is not a
normative part of this standard.  The example assumes that the product
is a direct-access device with an X3T10 Vendor ID of `XYZ_Corp', a
product identification of  `Super Turbo Disk', and a product serial
number of `2034589345'.  Furthermore, it is assumed that the
manufacturer has been assigned a 24-bit IEEE company_id of 01ABCDh by
the IEEE Registration Authority Committee and that the manufacture has
assigned a 24-bit extension_identifier of 234567h to this logical unit. 
The combined 48-bit identifier is reported in the 64-bit format as
defined by the IEEE 64-bit Global Identifier (EUI-64) standard.  The
data returned in the device identification VPD page for this logical
unit would be:

            Table 112 - Device identification page example
+========-====================================================-================+
| Bytes  | Hexadecimal Values                                 | ASCII Values   |
|========+====================================================+================|
| 00--15 | 00 83 00 32  02 01 00 22  58 59 5A 5F  43 6F 72 70 |...2..."XYZ_Corp|
| 16--31 | 53 75 70 65  72 20 54 75  72 62 6F 20  44 69 73 6B |Super Turbo Disk|
| 32--47 | 32 30 33 34  35 38 39 33  34 35 01 02  00 08 01 AB |2034589345......|
| 48--53 | CD FF FF 23  45 67                                 |......          |
|==============================================================================|
| Note: Non-printing ASCII characters are shown as '.'.                        |
+==============================================================================+

****************************************************************************/

struct inquiry_p83 {
    unsigned char   type_qual;
    unsigned char   page_code;
    unsigned char   Rsvd;
    unsigned char   page_len;
    unsigned char   idlist[];
};

struct p83_idlist {
    unsigned char   code_set;
    unsigned char   idtype;
    unsigned char   Rsvd;   
    unsigned char   page_len;
    unsigned char   id[];
};

#define P83_CODE_SET_RSVD   0
#define P83_CODE_SET_BINARY 1
#define P83_CODE_SET_ASCII  2

#define P83_ID_TYPE_NOAUTHORITY   0
#define P83_ID_TYPE_VENDORID      1
#define P83_ID_TYPE_IEEE_EXTENDED 2
#define P83_ID_TYPE_IEEE64        3


