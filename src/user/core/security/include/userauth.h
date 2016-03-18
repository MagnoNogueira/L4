#ifndef USERAUTH_H
#define USERAUTH_H

#include "sys_type.h"
#include "sys_cpnt.h"
#include "sys_adpt.h"
#include "sysfun.h"
#include "l_inet.h"
//#include "security_backdoor.h"

/* NAMING CONSTANT DECLARATIONS
 */
#define USERAUTH_SUPPORT_ACCTON_BACKDOOR      (FALSE && SECURITY_SUPPORT_ACCTON_BACKDOOR) /* support backdoor functions */

/* MACRO FUNCTION DECLARATIONS
 */

/* TYPE DECLARATIONS
 */

/* Define SNMP Community data type
 *
 * NOTES: 1. The SNMP community string can only be accessed by CLI and Web.
 *           SNMP management station CAN NOT access the SNMP community string.
 *        2. There is no MIB to define the SNMP community string.
 *        3. Status of each SNMP community is defined as following:
 *              - invalid(0): the entry of this community string is deleted/purged
 *              - enabled(1): this community string is enabled
 *              - disabled(2): this community string is disabled

 *           Set status to invalid(0) will delete/purge a community string.
 *
 *           Commuinty string name is an "ASCII Zero" string (char array ending with '\0').
 *
 *           Access right of community string could be
 *              - READ_ONLY(1)
 *              - READ_WRITE(2)
 *
 *        4. The total number of SNMP community string supported by the system
 *           is defined by SYS_ADPT_MAX_NBR_OF_SNMP_COMMUNITY_STRING.
 *        5. By default, two SNMM community string are configued:
 *              - "PUBLIC"      READ-ONLY       enabled
 *              - "PRIVATE"     READ-WRITE      enabled
 *        6. For any SNMP request command, all of enabled community string
 *           shall be used to authenticate/authorize if this SNMP request
 *           is legal/permit.
 */
typedef struct
{
    /* comm_string_name - (key) to specify a unique SNMP community string
     */
    UI8_T   comm_string_name[SYS_ADPT_MAX_COMM_STR_NAME_LEN + 1];

    UI32_T  access_right;
    UI32_T  status;

} USERAUTH_SnmpCommunity_T;


/* Define User/Password data type
 *
 * Notes: 1. The user/password can only be accessed by CLI and Web.
 *           SNMP management station CAN NOT access the user/password.
 *        2. There is no MIB to define the user/password.
 *        3. Cisco CLI supports up to 16 user privilege level, 0 ~ 15.
 *        4. Each user privilege level have a unique user/password assigned.
 *           So, the system can have max 16 user/password defined for each
 *           different privilege level.
 *        5. Status of each privilege level is defined as following:
 *              - invalid(0): the entry of this user/password is deleted/purged
 *              - valid(1): this user/password is valid
 *        6. The total number of user/password supported by the system
 *           is defined by SYS_ADPT_MAX_NBR_OF_USER_PASSWORD.
 *        7. Each time when user login to system via local console or telnet,
 *           all of valid(1) user/password shall be used to authenticate/authorize
 *           the user.
 *        8. Basically, privilege will count from 1 by internal design, but what
 *           user sees will be count from 0.  The reason why we design like this
 *           is for GetNextRunningCfg.  Caller will try to get first entry by
 *           set privilege 0 and we will return privilege 1's record.  But whenever
 *           CLI display to user, we need to show privilege 0.
 *        9. The difference between password and privilege password is
 *           a) password is relative with username, it's one to one mapping.
 *              and is used for log in only.
 *           b) If low privilege user wants to have higher privilede to access,
 *              he needs to know the relative privilege_password to get in.
 *              For CLI user, the "enable level 5" command will get a response to
 *              ask user to input a password.  Right now, privilege_password needs
 *              to be input as the key to get in.
 *       10. Status only tells that username and password is valid or not.  The
 *           privilege_password is not impacted by status.
 *       11. Higher privilege user can always change low level privilege username,
 *           password and privilege password.
 *       12. If all user name, password and privilege password are all "NO" by user,
 *           then telnet user can not log in any more, but CLI user still can log in
 *           and just type null user name, null password to get in to privilege mode.
 *       13. If highest level username, password is "NO" by user himself, then
 *           user still can get in to this level by using "enable password" and
 *           enter highest privilege password to get in.
 */
typedef struct
{
    /* username - (key) to specify a unique user name/password.
     */
    UI8_T       username[SYS_ADPT_MAX_USER_NAME_LEN + 1];

    UI8_T       password[SYS_ADPT_MAX_ENCRYPTED_PASSWORD_LEN + 1];
    UI32_T      privilege;
    UI32_T      status;
} USERAUTH_LoginLocal_T;


typedef struct
{
    /* privilege - (key) to specify a unique privilege.
     */
    UI32_T      privilege;

    UI8_T       password[SYS_ADPT_MAX_ENCRYPTED_PASSWORD_LEN + 1];
    UI32_T      status;
} USERAUTH_Privilege_Password_T;


typedef enum
{
    USERAUTH_ACCESS_RIGHT_READ_ONLY  = 1,
    USERAUTH_ACCESS_RIGHT_READ_WRITE = 2
} USERAUTH_Snmp_Comm_Access_Right_T;


typedef enum
{
    USERAUTH_ENTRY_INVALID = 0,
    USERAUTH_ENTRY_VALID = 1
} USERAUTH_Entry_T;

/* design changed for TACACS by JJ, 2002-05-24 */
typedef enum
{
    USERAUTH_AUTH_NONE                = 0,
    USERAUTH_AUTH_LOCAL               = 1,
    USERAUTH_AUTH_RADIUS              = 2,
    USERAUTH_AUTH_TACACS              = 3,
} USERAUTH_Auth_Method_T;

#define USERAUTH_AUTH_METHOD_STRINGS    \
    {                                   \
        "None",                         \
        "Local",                        \
        "Radius",                       \
        "Tacacs"                        \
    }

#define USERAUTH_NUMBER_Of_AUTH_METHOD  3

typedef enum
{
    USERAUTH_UNKNOWN   = USERAUTH_AUTH_NONE,
    USERAUTH_PASSWORD,
    USERAUTH_PUBLIC_KEY
} USERAUTH_Method_T;

#define USERAUTH_METHOD_STRINGS         \
    {                                   \
        "Unknown",                      \
        "Password",                     \
        "Public-Key"                    \
    }

/*----------------------------------------------*/

typedef enum
{
    USERAUTH_SESSION_CONSOLE          = 0,
    USERAUTH_SESSION_TELNET           = 1,
    USERAUTH_SESSION_WEB              = 2,
    USERAUTH_SESSION_ALL              = 3
} USERAUTH_SessionType_T;

#define USERAUTH_NUMBER_Of_SESSION_TYPE  3

typedef enum
{
    USERAUTH_LOGIN_NO_LOGIN     = 1,
    USERAUTH_LOGIN_LOGIN        = 2,
    USERAUTH_LOGIN_LOGIN_LOCAL  = 3,
} USERAUTH_Login_Method_T;

typedef enum
{
    USERAUTH_REJECT,
    USERAUTH_PASS,
    USERAUTH_USER_NOT_EXIST,
    USERAUTH_PROCESSING,
    USERAUTH_WRONG_COMMAND,
    USERAUTH_IPC_FAILED,
    USERAUTH_NOT_MASTER_MODE
} USERAUTH_ReturnValue_T;

/*for EH *//*Mercury_V2-00474*/
#define	USERAUTH_GETSNMPCOMMUNITY_FUNC_NO		1
#define USERAUTH_GETnextSNMPCOMMUNITY_FUNC_NO		2
#define USERAUTH_SETSNMPCOMMUNITYACCESSRIGHT_FUNC_NO	3
#define USERAUTH_SETSNMPCOMMUNITYSTATUS_FUNC_NO		4
#define USERAUTH_GETLOGINLOCALUSER_FUNC_NO		5
#define USERAUTH_GETNEXTLOGINLOCALUSER_FUNC_NO		6
#define USERAUTH_GETNEXTRUNNINGLOGINLOCALUSER_FUNC_NO	7
#define USERAUTH_SETLOGINLOCALUSERSTATUS_FUNC_NO	8
#define USERAUTH_SETLOGINLOCALUSERPASSWORD_FUNC_NO	9
#define USERAUTH_SETLOGINLOCALUSERPRIVILEGE_FUNC_NO	10
#define USERAUTH_GETPRIVILEGEPASSWORD_FUNC_NO		11
#define USERAUTH_GETNEXTPRIVILEGEPASSWORD_FUNC_NO	12
#define USERAUTH_GETFIRSTRUNNINGPRIVILEGEPASSWORD_FUNC_NO	13
#define USERAUTH_GETNEXTRUNNINGPRIVILEGEPASSWORD_FUNC_NO	14
#define USERAUTH_SETtPRIVILEGEPASSWORDSTATUS_FUNC_NO	15
#define USERAUTH_SETPRIVILEGEPASSWORD_FUNC_NO		16
#define USERAUTH_GETSNMPIPFILTER_FUNC_NO		17
#define USERAUTH_GETNEXTSNMPIPFILTER_FUNC_NO		18
#define USERAUTH_SETSNMPIPFILTERSTATUS_FUNC_NO		19
#define USERAUTH_ISVALISSNMPIPFILTERADDRESS_FUNC_NO	20
#define USERAUTH_CONSOLEENABLEPASSWORDAUTH_FUNC_NO	21
#define USERAUTH_TELNETENABLEPASSWORDAUTH_FUNC_NO	22

#define USERAUTH_GUEST_USER_PRIVILEGE           0
#define USERAUTH_MANAGER_USER_PRIVILEGE         8
#define USERAUTH_ADMIN_USER_PRIVILEGE           15

#ifdef SYS_ADPT_USERAUTH_SUPER_USER_NAME
#define USERAUTH_SUPERUSER_USERNAME_DEF         SYS_ADPT_USERAUTH_SUPER_USER_NAME
#else
#define USERAUTH_SUPERUSER_USERNAME_DEF         "__super"
#endif

/* Mark by allen_lee, 2002.03.25 */
/* The password of superuser will be got from the function according to Mac address*/
/*#define USERAUTH_SUPERUSER_PASSWORD_DEF         "__super"*/
#define USERAUTH_SUPERUSER_USER_PRIVILEGE       16

#define USERAUTH_SNMP_COMM_READ_ONLY_STR_DEF    "public"
#define USERAUTH_SNMP_COMM_READ_WRITE_STR_DEF   "private"


#define USERAUTH_AUTH_METHOD_DEF                USERAUTH_AUTH_LOCAL_ONLY
#define USERAUTH_LOGIN_METHOD_DEF               USERAUTH_LOGIN_LOGIN_LOCAL


/*isiah.2003-05-26.*/
#if (SYS_CPNT_3COM_PASSWORD_RECOVERY == TRUE)
typedef struct
{
    BOOL_T  active;
    UI32_T  status;
}USERAUTH_PasswordRecovery_T;

typedef enum
{
    USERAUTH_PASSWORD_RECOVERY_ENABLED = 1,
    USERAUTH_PASSWORD_RECOVERY_DISABLED = 2
}USERAUTH_PasswordRecoveryStatus_T;
#endif

typedef enum
{
    USERAUTH_ASYNC_LOGIN_AUTH_REQUEST,
    USERAUTH_ASYNC_LOGIN_AUTH_RESPONSE,

    USERAUTH_ASYNC_ENABLE_AUTH_REQUEST,
    USERAUTH_ASYNC_ENABLE_AUTH_RESPONSE,
} USERAUTH_AsyncAuthCmd_T;

typedef struct
{
    UI32_T                          privilege;
    USERAUTH_Auth_Method_T          authen_by_whom;
    USERAUTH_Method_T               method;
} USERAUTH_AuthResult_T;

typedef struct
{
    I32_T                           status;         /* Explain per method */
    UI32_T                          privilege;
    USERAUTH_Auth_Method_T          authen_by_whom;
} USERAUTH_AsyncAuthenResult_T;

typedef struct
{
    /* request info
     */
    char                            name[SYS_ADPT_MAX_USER_NAME_LEN + 1];
    char                            password[SYS_ADPT_MAX_PASSWORD_LEN + 1];
    union
    {
        UI32_T                      ifindex;
        struct
        {
            USERAUTH_SessionType_T  sess_type;
            UI32_T                  sess_id;
        };
    };
    L_INET_AddrIp_T                 rem_ip_addr;
    UI32_T                          privilege;      /* request privilege */

    /* result
     */
    USERAUTH_AsyncAuthenResult_T    result;

    /* internal use
     */
    USERAUTH_AsyncAuthCmd_T         cmd;
    USERAUTH_Auth_Method_T          auth_method_list[USERAUTH_NUMBER_Of_AUTH_METHOD];
    UI32_T                          curr_auth_index;

    UI32_T                          cookie; /* IPC, waitting command number */
} USERAUTH_AsyncAuthParam_T;

/* definitions of command which will be used in ipc message
 */
enum
{
    USERAUTH_IPCCMD_GETNEXTSNMPCOMMUNITY,
    USERAUTH_IPCCMD_GETNEXTRUNNINGSNMPCOMMUNITY,
    USERAUTH_IPCCMD_SETSNMPCOMMUNITYACCESSRIGHT,
    USERAUTH_IPCCMD_SETSNMPCOMMUNITYSTATUS,
    USERAUTH_IPCCMD_GETLOGINLOCALUSER,
    USERAUTH_IPCCMD_GETNEXTLOGINLOCALUSER,
    USERAUTH_IPCCMD_GETNEXTRUNNINGLOGINLOCALUSER,
    USERAUTH_IPCCMD_SETLOGINLOCALUSERSTATUS,
    USERAUTH_IPCCMD_SETLOGINLOCALUSERPASSWORD,
    USERAUTH_IPCCMD_SETLOGINLOCALUSERPRIVILEGE,
    USERAUTH_IPCCMD_GETPRIVILEGEPASSWORD,
    USERAUTH_IPCCMD_GETFIRSTRUNNINGPRIVILEGEPASSWORD,
    USERAUTH_IPCCMD_GETNEXTRUNNINGPRIVILEGEPASSWORD,
    USERAUTH_IPCCMD_SETPRIVILEGEPASSWORDSTATUS,
    USERAUTH_IPCCMD_SETPRIVILEGEPASSWORD,
    USERAUTH_IPCCMD_GETAUTHMETHOD,
    USERAUTH_IPCCMD_SETAUTHMETHOD,
    USERAUTH_IPCCMD_GETRUNNINGAUTHMETHOD,
    USERAUTH_IPCCMD_GETENABLEAUTHMETHOD,
    USERAUTH_IPCCMD_SETENABLEAUTHMETHOD,
    USERAUTH_IPCCMD_GETRUNNINGENABLEAUTHMETHOD,
    USERAUTH_IPCCMD_GETTELNETLOGINMETHOD,
    USERAUTH_IPCCMD_SETTELNETLOGINMETHOD,
    USERAUTH_IPCCMD_GETRUNNINGTELNETLOGINMETHOD,
    USERAUTH_IPCCMD_GETCONSOLLOGINMETHOD,
    USERAUTH_IPCCMD_SETCONSOLELOGINMETHOD,
    USERAUTH_IPCCMD_GETRUNNINGCONSOLELOGINMETHOD,
    USERAUTH_IPCCMD_GETCONSOLELOGINPASSWORD,
    USERAUTH_IPCCMD_SETCONSOLELOGINPASSWORD,
    USERAUTH_IPCCMD_GETRUNNINGCONSOLELOGINPASSWORD,
    USERAUTH_IPCCMD_GETTELNETLOGINPASSWORD,
    USERAUTH_IPCCMD_SETTELNETLOGINPASSWORD,
    USERAUTH_IPCCMD_GETRUNNINGTELNETLOGINPASSWORD,
    USERAUTH_IPCCMD_LOGINAUTH,
    USERAUTH_IPCCMD_LOGINAUTHPASSBYWHICHMETHOD,
    USERAUTH_IPCCMD_SETPASSWORDRECOVERYACTIVE,
    USERAUTH_IPCCMD_GETPASSWORDRECOVERYACTIVE,
    USERAUTH_IPCCMD_DOPASSWORDRECOVERY,
    USERAUTH_IPCCMD_SETAUTHMETHODBYSESSIONTYPE,
    USERAUTH_IPCCMD_GETAUTHMETHODBYSESSIONTYPE,
    USERAUTH_IPCCMD_LOGIN_AUTH_BY_SESSION_TYPE,
    USERAUTH_IPCCMD_ENABLE_PASSWORD_AUTH,

    USERAUTH_IPCCMD_FOLLOWISASYNCHRONISMIPC,
};

/*use to the definition of IPC message buffer*/
typedef struct
{
    union
    {
        UI32_T cmd;          /*cmd fnction id*/
        BOOL_T result_bool;  /*respond bool return*/
        UI32_T result_ui32;  /*respond ui32 return*/
        I32_T result_i32;  /*respond i32 return*/   /*maggie liu for RADIUS authentication ansync*/
    }type;

    union
    {

        BOOL_T bool_v;
        UI8_T  ui8_v;
        I8_T   i8_v;
        UI32_T ui32_v;
        UI16_T ui16_v;
        I32_T i32_v;
        I16_T i16_v;
        UI8_T ip4_v[4];

        USERAUTH_SnmpCommunity_T comm_entry;

        struct
        {
            UI8_T comm_string_name[SYS_ADPT_MAX_COMM_STR_NAME_LEN + 1];
            UI32_T access_right;
        }comm_access;

        struct
        {
            UI8_T comm_string_name[SYS_ADPT_MAX_COMM_STR_NAME_LEN + 1];
            UI32_T status;
        }comm_status;

        USERAUTH_LoginLocal_T login_user;

        struct
        {
            UI8_T username[SYS_ADPT_MAX_USER_NAME_LEN+1];
            UI32_T status;
        }user_status;

        struct
        {
            char username[SYS_ADPT_MAX_USER_NAME_LEN+1];
            char password[SYS_ADPT_MAX_ENCRYPTED_PASSWORD_LEN + 1];
        }user_password;

        struct
        {
            char username[SYS_ADPT_MAX_USER_NAME_LEN+1];
            UI32_T privilege;
        }user_privilege;

        USERAUTH_Privilege_Password_T privilege_password;

        struct
        {
            UI32_T ui32_a1;
            UI32_T ui32_a2;
        }ui32a1_ui32a2;

        USERAUTH_Auth_Method_T auth_method;

        USERAUTH_Auth_Method_T auth_method_numbers[USERAUTH_NUMBER_Of_AUTH_METHOD];

        USERAUTH_Login_Method_T login_method;

        char password[SYS_ADPT_MAX_ENCRYPTED_PASSWORD_LEN+1];

        struct
        {
            char name[SYS_ADPT_MAX_USER_NAME_LEN+1];
            char password[SYS_ADPT_MAX_PASSWORD_LEN+1];
            UI32_T priv;
            USERAUTH_SessionType_T session_type;
            UI32_T sess_id;
            L_INET_AddrIp_T rem_ip_addr;
        }name_password_priv;

        union
        {
            struct
            {
                char name[SYS_ADPT_MAX_USER_NAME_LEN+1];
                char password[SYS_ADPT_MAX_PASSWORD_LEN+1];
                USERAUTH_SessionType_T sess_type;
                UI32_T sess_id;
                L_INET_AddrIp_T rem_ip_addr;
            } req;

            struct
            {
                USERAUTH_AuthResult_T   result;
            } resp;
        } login_auth;

        union
        {
            struct
            {
                char name[SYS_ADPT_MAX_USER_NAME_LEN+1];
                char password[SYS_ADPT_MAX_PASSWORD_LEN+1];
                UI32_T privilege;
                USERAUTH_SessionType_T sess_type;
                UI32_T sess_id;
                L_INET_AddrIp_T rem_ip_addr;
            } req;

            struct
            {
                USERAUTH_AuthResult_T   result;
            } resp;
        } enable_auth;

        union
        {
            struct
            {
                UI32_T ifindex;
                L_INET_AddrIp_T rem_ip_addr;
                char username[SYS_ADPT_MAX_USER_NAME_LEN + 1];
                char password[SYS_ADPT_MAX_PASSWORD_LEN + 1];
                UI32_T privilege;
            } req;

            struct
            {
                UI32_T  privilege;
            } resp;
        } login_author;

        USERAUTH_AuthResult_T   auth_result;

        struct
        {
            UI32_T ifindex;
            L_INET_AddrIp_T rem_ip_addr;
            char username[SYS_ADPT_MAX_USER_NAME_LEN + 1];
            char password[SYS_ADPT_MAX_PASSWORD_LEN + 1];
            UI32_T privilege;
        }if_name_password_privilege;

        struct
        {
            UI8_T username[SYS_ADPT_MAX_USER_NAME_LEN + 1];
            UI32_T privilege;
        }name_privilege;

        struct
        {
            char name[SYS_ADPT_MAX_USER_NAME_LEN+1];
            char password[SYS_ADPT_MAX_PASSWORD_LEN+1];
            UI32_T sess_type;
            UI32_T priv;
        }name_password_type_priv;

        struct
        {
            USERAUTH_Auth_Method_T auth_method_numbers[USERAUTH_NUMBER_Of_AUTH_METHOD];
            UI32_T sess_type;
        }method_type;

        /**
         ** Use for receiving event from sys_callback to make sure sys_mgmt_group
         ** have enough buffer to save this event message.
         **/
        struct
        {
            USERAUTH_AsyncAuthenResult_T    result;
            USERAUTH_AsyncAuthParam_T       cookie;
            UI32_T                          cookie_size;
        } _annonuce_rem_srv_auth_result;

    } data; /* contains the supplemntal data for the corresponding cmd */
}USERAUTH_IPCMsg_T;

#define USERAUTH_MSGBUF_TYPE_SIZE    sizeof(((USERAUTH_IPCMsg_T *)0)->type)


/* EXPORTED SUBPROGRAM SPECIFICATIONS
 */

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - USERAUTH_Init
 * ---------------------------------------------------------------------
 * PURPOSE: This function will init the system resource
 *
 * INPUT: None
 * OUTPUT: None
 * RETURN: TRUE/FALSE
 * NOTES: None
 * ---------------------------------------------------------------------
 */
BOOL_T USERAUTH_Init(void);

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - USERAUTH_Create_InterCSC_Relation
 * ---------------------------------------------------------------------
 * PURPOSE: This function initializes all function pointer registration operations.
 *
 * INPUT:  None
 * OUTPUT: None
 * RETURN: None
 * NOTES:  None
 * ---------------------------------------------------------------------
 */
void USERAUTH_Create_InterCSC_Relation(void);

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - USERAUTH_SetTransitionMode
 * ---------------------------------------------------------------------
 * PURPOSE: This function will set the system in Transition MOde
 *
 * INPUT: None
 * OUTPUT: None
 * RETURN: None
 * NOTES: None
 * ---------------------------------------------------------------------
 */
void USERAUTH_SetTransitionMode(void);

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - USERAUTH_EnterTransitionMode
 * ---------------------------------------------------------------------
 * PURPOSE: This function will init the system info
 *
 * INPUT: None
 * OUTPUT: None
 * RETURN: None
 * NOTES: None
 * ---------------------------------------------------------------------
 */
void USERAUTH_EnterTransitionMode(void);


/* ---------------------------------------------------------------------
 * ROUTINE NAME  - USERAUTH_EnterMasterMode
 * ---------------------------------------------------------------------
 * PURPOSE: This function will init the system info
 *
 * INPUT: None
 * OUTPUT: None
 * RETURN: None
 * NOTES: None
 * ---------------------------------------------------------------------
 */
void USERAUTH_EnterMasterMode(void);


/* ---------------------------------------------------------------------
 * ROUTINE NAME  - USERAUTH_EnterSlaveMode
 * ---------------------------------------------------------------------
 * PURPOSE: This function will init the system info
 *
 * INPUT: None
 * OUTPUT: None
 * RETURN: None
 * NOTES: None
 * ---------------------------------------------------------------------
 */
void USERAUTH_EnterSlaveMode(void);

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - USERAUTH_ProvisionComplete
 * ---------------------------------------------------------------------
 * PURPOSE: This function will be called when provision complete
 * INPUT: None
 * OUTPUT: None
 * RETURN: None
 * NOTES: None
 * ---------------------------------------------------------------------
 */
void USERAUTH_ProvisionComplete(void);

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - USERAUTH_GetSnmpCommunity
 * ---------------------------------------------------------------------
 * PURPOSE: This function returns true if the specified SNMP community
 *          string can be retrieved successfully. Otherwise, false is returned.
 *
 * INPUT: comm_entry->comm_string_name - (key) to specify a unique SNMP community string
 * OUTPUT: comm_string                  - SNMP community info
 * RETURN: TRUE/FALSE
 * NOTES: 1. The SNMP community string can only be accessed by CLI and Web.
 *           SNMP management station CAN NOT access the SNMP community string.
 *        2. There is no MIB to define the SNMP community string.
 *        3. Status of each SNMP community is defined as following:
 *              - invalid(0): the entry of this community string is deleted/purged
 *              - enabled(1): this community string is enabled
 *              - disabled(2): this community string is disabled

 *           Set status to invalid(0) will delete/purge a community string.
 *
 *           Commuinty string name is an "ASCII Zero" string (char array ending with '\0').
 *
 *           Access right of community string could be
 *              - READ_ONLY(1)
 *              - READ_WRITE(2)
 *
 *        4. The total number of SNMP community string supported by the system
 *           is defined by SYS_ADPT_MAX_NBR_OF_SNMP_COMMUNITY_STRING.
 *        5. By default, two SNMM community string are configued:
 *              - "PUBLIC"      READ-ONLY       enabled
 *              - "PRIVATE"     READ-WRITE      enabled
 *        6. For any SNMP request command, all of enabled community string
 *           shall be used to authorize if this SNMP request is legal/permit.
 * ---------------------------------------------------------------------
 */
BOOL_T USERAUTH_GetSnmpCommunity(USERAUTH_SnmpCommunity_T *comm_entry);


/* ---------------------------------------------------------------------
 * ROUTINE NAME  - USERAUTH_GetNextSnmpCommunity
 * ---------------------------------------------------------------------
 * PURPOSE: This function returns true if the next available SNMP community
 *          string can be retrieved successfully. Otherwise, false is returned.
 *
 * INPUT: comm_entry->comm_string_name - (key) to specify a unique SNMP community string
 * OUTPUT: comm_string                  - next available SNMP community info
 * RETURN: TRUE/FALSE
 * NOTES: 1. Commuinty string name is an "ASCII Zero" string (char array ending with '\0').
 *        2. Any invalid(0) community string will be skip duing the GetNext operation.
 */
BOOL_T USERAUTH_GetNextSnmpCommunity(USERAUTH_SnmpCommunity_T *comm_entry);


/* ---------------------------------------------------------------------
 * ROUTINE NAME  - USERAUTH_GetNextRunningSnmpCommunity
 * ---------------------------------------------------------------------
 * PURPOSE: This function returns SYS_TYPE_GET_RUNNING_CFG_SUCCESS if the
 *          next available non-default SNMP community can be retrieved
 *          successfully. Otherwise, SYS_TYPE_GET_RUNNING_CFG_FAIL
 *          or SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE is returned.
 * INPUT: comm_entry->comm_string_name - (key) to specify a unique SNMP community
 * OUTPUT: snmp_comm                  - next available non-default SNMP community
 * RETURN: SYS_TYPE_GET_RUNNING_CFG_SUCCESS, SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE, or
 *         SYS_TYPE_GET_RUNNING_CFG_FAIL
 * NOTES: 1. This function shall only be invoked by CLI to save the
 *           "running configuration" to local or remote files.
 *        2. Since only non-default configuration will be saved, this
 *           function shall return non-default SNMP community.
 *        3. Community string name is an "ASCII Zero" string (char array ending with '\0').
 *        4. Any invalid(0) SNMP community will be skip during the GetNext operation.
 * ---------------------------------------------------------------------
 */
UI32_T USERAUTH_GetNextRunningSnmpCommunity(USERAUTH_SnmpCommunity_T *comm_entry);


/* ---------------------------------------------------------------------
 * ROUTINE NAME  - USERAUTH_SetSnmpCommunityAccessRight
 * ---------------------------------------------------------------------
 * PURPOSE: This function returns true if the access right can be successfully
 *          set to the specified community string. Otherwise, false is returned.
 *
 * INPUT: comm_string_name  - (key) to specify a unique SNMP community string
 *        access_right      - the access level for this SNMP community
 * OUTPUT: None
 * RETURN: TRUE/FALSE
 * NOTES: 1. This function will create a new community string to the system
 *           if the specified comm_string_name does not exist, and total number
 *           of community string configured is less than
 *           SYS_ADPT_MAX_NBR_OF_SNMP_COMMUNITY_STRING.
 *           When a new community string is created by this function, the
 *           status of this new community string will be set to disabled(2)
 *           by default.
 *        2. This function will update the access right an existed community
 *           string if the specified comm_string_name existed already.
 *        3. False is returned if total number of community string configured
 *           is greater than SYS_ADPT_MAX_NBR_OF_SNMP_COMMUNITY_STRING
 * ---------------------------------------------------------------------
 */
BOOL_T USERAUTH_SetSnmpCommunityAccessRight(UI8_T *comm_string_name, UI32_T access_right);

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - USERAUTH_SetSnmpCommunityStatus
 * ---------------------------------------------------------------------
 * PURPOSE: This function returns true if the status can be successfully
 *          set to the specified community string. Otherwise, false is returned.
 *
 * INPUT: comm_string_name  - (key) to specify a unique SNMP community string
 *        status            - the status for this SNMP community(enabled/disabled)
 * OUTPUT: None
 * RETURN: TRUE/FALSE
 * NOTES: 1. This function will create a new community string to the system if
 *           the specified comm_string_name does not exist, and total number
 *           of community string configured is less than
 *           SYS_ADPT_MAX_NBR_OF_SNMP_COMMUNITY_STRING.
 *           When a new community string is created by this function, the
 *           access right of this new community string will be set to READ_ONLY(1)
 *           by default.
 *        2. This function will update an existed community string if
 *           the specified comm_string_name existed already.
 *        3. False is returned if total number of community string configured
 *           is greater than SYS_ADPT_MAX_NBR_OF_SNMP_COMMUNITY_STRING
 * ---------------------------------------------------------------------
 */
BOOL_T USERAUTH_SetSnmpCommunityStatus(UI8_T *comm_string_name, UI32_T status);




/* ---------------------------------------------------------------------
 *  ROUTINE NAME  - USERAUTH_GetLoginUser
 * ---------------------------------------------------------------------
 *  FUNCTION: This function gets login user's entry by specified user name
 *  INPUT   : login_user->username (key) in the record
 *  OUTPUT  : login_user a whole record on specified user name
 *  RETURN	: BOOL_T True : Get successfully, False : Get failed
 * ---------------------------------------------------------------------
 *  NOTE    : If user use the key of super user's username, this function
 *            will return the inhibited super user record.
 */
BOOL_T USERAUTH_GetLoginLocalUser(USERAUTH_LoginLocal_T *login_user);


/* ---------------------------------------------------------------------
 *  ROUTINE NAME  - USERAUTH_GetNextLoginUser
 * ---------------------------------------------------------------------
 *  FUNCTION: This function gets next legal login local user's entry
 *  INPUT   : login_user->username (key) in the record
 *  OUTPUT  : login_user a whole record on specified user name
 *  RETURN	: BOOL_T True : Get successfully, False : Get failed
 * ---------------------------------------------------------------------
 *  NOTE    : None
 */
BOOL_T USERAUTH_GetNextLoginLocalUser(USERAUTH_LoginLocal_T *login_user);


/* ---------------------------------------------------------------------
 * ROUTINE NAME  - USERAUTH_GetNextRunningLoginLocalUser
 * ---------------------------------------------------------------------
 * PURPOSE: This function returns SYS_TYPE_GET_RUNNING_CFG_SUCCESS if the
 *          next available non-default user/password can be retrieved
 *          successfully. Otherwise, SYS_TYPE_GET_RUNNING_CFG_FAIL
 *          or SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE is returned.
 * INPUT: login_user->privilege  - (key) to specify a unique user name
 * OUTPUT: user_password            - next available non-default user/password info
 * RETURN: SYS_TYPE_GET_RUNNING_CFG_SUCCESS, SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE, or
 *         SYS_TYPE_GET_RUNNING_CFG_FAIL
 * NOTES: 1. This function shall only be invoked by CLI to save the
 *           "running configuration" to local or remote files.
 *        2. Since only non-default configuration will be saved, this
 *           function shall return non-default user/password.
 * ---------------------------------------------------------------------
 */
UI32_T USERAUTH_GetNextRunningLoginLocalUser(USERAUTH_LoginLocal_T *login_user);


/* ---------------------------------------------------------------------
 *  ROUTINE NAME  - USERAUTH_SetLoginLocalUserStatus
 * ---------------------------------------------------------------------
 *  FUNCTION: This function sets username password entry status based on
 *            specified privilege.  If the username is not in the database,
 *            we will create a new user if the user's number is not exceed
 *            the limit.
 *  INPUT   : username (key)
 *          : status(USERAUTH_ENTRY_VALID/USERAUTH_ENTRY_INVALID)
 *  OUTPUT  : username
 *  RETURN	: BOOL_T True : Get successfully, False : Get failed
 * ---------------------------------------------------------------------
 *  NOTE    : None
 */
BOOL_T USERAUTH_SetLoginLocalUserStatus(UI8_T *username, UI32_T status);


/* ---------------------------------------------------------------------
 *  ROUTINE NAME  - USERAUTH_SetLoginLocalUserPassword
 * ---------------------------------------------------------------------
 *  FUNCTION: This function sets password based on specified privilege
 *  INPUT   : username (key)
 *          : password
 *  OUTPUT  : None
 *  RETURN	: BOOL_T True : Get successfully, False : Get failed
 * ---------------------------------------------------------------------
 *  NOTE    : None
 */
BOOL_T USERAUTH_SetLoginLocalUserPassword(const char *username, const char *password);


/* ---------------------------------------------------------------------
 *  ROUTINE NAME  - USERAUTH_SetLoginUserPrivilege
 * ---------------------------------------------------------------------
 *  FUNCTION: This function sets password based on specified privilege
 *  INPUT   : username (key)
 *          : password
 *          : privilege
 *  OUTPUT  : None
 *  RETURN	: BOOL_T True : Get successfully, False : Get failed
 * ---------------------------------------------------------------------
 *  NOTE    : None
 */
BOOL_T USERAUTH_SetLoginLocalUserPrivilege(const char *username, UI32_T privilege);



/* ---------------------------------------------------------------------
 *  ROUTINE NAME  - USERAUTH_GetPrivilegePassword
 * ---------------------------------------------------------------------
 *  FUNCTION: This function gets privilege password based on specified
 *            privilege
 *  INPUT   : privilege (key)
 *  OUTPUT  : privilege_password in record
 *  RETURN	: BOOL_T True : Get successfully, False : Get failed
 * ---------------------------------------------------------------------
 *  NOTE    : None
 */
BOOL_T USERAUTH_GetPrivilegePassword(USERAUTH_Privilege_Password_T *privilege_password);


/* ---------------------------------------------------------------------
 *  ROUTINE NAME  - USERAUTH_GetNextPrivilegePassword
 * ---------------------------------------------------------------------
 *  FUNCTION: This function gets next valid privilege password based on
 *            specified privilege
 *  INPUT   : privilege (key)
 *  OUTPUT  : privilege_password in record
 *  RETURN	: BOOL_T True : Get successfully, False : Get failed
 * ---------------------------------------------------------------------
 *  NOTE    : None
 */
BOOL_T USERAUTH_GetNextPrivilegePassword(USERAUTH_Privilege_Password_T *privilege_password);


/* ---------------------------------------------------------------------
 *  ROUTINE NAME  - USERAUTH_GetFirstRunningPrivilegePassword
 * ---------------------------------------------------------------------
 * PURPOSE: This function returns SYS_TYPE_GET_RUNNING_CFG_SUCCESS if the
 *          next available non-default privilege password can be retrieved
 *          successfully. Otherwise, SYS_TYPE_GET_RUNNING_CFG_FAIL
 *          or SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE is returned.
 * INPUT : privilege_password->privilege  - (key) to specify a unique user/password
 * OUTPUT: password            - next available non-default password info
 * RETURN: SYS_TYPE_GET_RUNNING_CFG_SUCCESS, SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE, or
 *         SYS_TYPE_GET_RUNNING_CFG_FAIL
 * NOTES: 1. This function shall only be invoked by CLI to save the
 *           "running configuration" to local or remote files.
 *        2. Since only non-default configuration will be saved, this
 *           function shall return non-default privilege password.
 *        3. This function is a very particular API.  Since the privilege
 *           is count from 0.  But if we only support GetNext, then we can
 *           not get the first one.  So this is the Get First Running Config.
 */
BOOL_T USERAUTH_GetFirstRunningPrivilegePassword(USERAUTH_Privilege_Password_T *privilege_password);


/* ---------------------------------------------------------------------
 *  ROUTINE NAME  - USERAUTH_GetNextRunningPrivilegePassword
 * ---------------------------------------------------------------------
 * PURPOSE: This function returns SYS_TYPE_GET_RUNNING_CFG_SUCCESS if the
 *          next available non-default privilege password can be retrieved
 *          successfully. Otherwise, SYS_TYPE_GET_RUNNING_CFG_FAIL
 *          or SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE is returned.
 * INPUT : privilege_password->privilege  - (key) to specify a unique user/password
 * OUTPUT: password            - next available non-default password info
 * RETURN: SYS_TYPE_GET_RUNNING_CFG_SUCCESS, SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE, or
 *         SYS_TYPE_GET_RUNNING_CFG_FAIL
 * NOTES: 1. This function shall only be invoked by CLI to save the
 *           "running configuration" to local or remote files.
 *        2. Since only non-default configuration will be saved, this
 *           function shall return non-default user/password.
 */
BOOL_T USERAUTH_GetNextRunningPrivilegePassword(USERAUTH_Privilege_Password_T *privilege_password);


/* ---------------------------------------------------------------------
 *  ROUTINE NAME  - USERAUTH_SetPrivilegePasswordStatus
 * ---------------------------------------------------------------------
 *  FUNCTION: This function sets privilege password based on specified
 *            privilege
 *  INPUT   : privilege (key)
 *            privilege_password
 *  OUTPUT  : status(USERAUTH_ENTRY_VALID/USERAUTH_ENTRY_INVALID)
 *  RETURN	: BOOL_T True : Get successfully, False : Get failed
 * ---------------------------------------------------------------------
 *  NOTE    : None
 */
BOOL_T USERAUTH_SetPrivilegePasswordStatus(UI32_T privilege, UI32_T status);


/* ---------------------------------------------------------------------
 *  ROUTINE NAME  - USERAUTH_SetPrivilegePassword
 * ---------------------------------------------------------------------
 *  FUNCTION: This function sets privilege password based on specified
 *            privilege
 *  INPUT   : privilege (key)
 *            password
 *  OUTPUT  : None
 *  RETURN	: BOOL_T True : Get successfully, False : Get failed
 * ---------------------------------------------------------------------
 *  NOTE    : None
 */
BOOL_T USERAUTH_SetPrivilegePassword(UI32_T privilege, UI8_T *password);


/* ---------------------------------------------------------------------
 *  ROUTINE NAME  - USERAUTH_GetAuthMethod
 * ---------------------------------------------------------------------
 *  FUNCTION: This function gets authentication method
 *  INPUT   : auth_method (memory buffer)
 *  OUTPUT  : auth_method
 *  RETURN	: BOOL_T True : Get successfully, False : Get failed
 *  Note: auth_method is changed to an array of AUTH_METHOD_T
 *           (JJ, 2002-05-24)
 * ---------------------------------------------------------------------
 */
BOOL_T USERAUTH_GetAuthMethod(USERAUTH_Auth_Method_T *auth_method);


/* ---------------------------------------------------------------------
 *  ROUTINE NAME  - USERAUTH_SetAuthMethod
 * ---------------------------------------------------------------------
 *  FUNCTION: This function sets authentication method
 *  INPUT   : auth_method
 *  OUTPUT  : None
 *  RETURN	: BOOL_T True : Set successfully, False : Set failed
 *  Note: auth_method is changed to an array of AUTH_METHOD_T
 *           (JJ, 2002-05-24)
 * ---------------------------------------------------------------------
 */
BOOL_T USERAUTH_SetAuthMethod(USERAUTH_Auth_Method_T *auth_method);


/* ---------------------------------------------------------------------
 *  ROUTINE NAME  - USERAUTH_GetRunningAuthMethod
 * ---------------------------------------------------------------------
 * PURPOSE: This function returns SYS_TYPE_GET_RUNNING_CFG_SUCCESS if the
 *          authentication method is changed.  Otherwise,
 *          SYS_TYPE_GET_RUNNING_CFG_FAIL
 *          or SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE is returned.
 * INPUT : auth_method (memory buffer)
 * OUTPUT: auth_method
 * RETURN: SYS_TYPE_GET_RUNNING_CFG_SUCCESS, SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE, or
 *         SYS_TYPE_GET_RUNNING_CFG_FAIL
 * NOTES: 1. This function shall only be invoked by CLI to save the
 *           "running configuration" to local or remote files.
 *        2. Since only non-default configuration will be saved, this
 *           function shall return non-default privilege password.
 *  Note: auth_method is changed to an array of AUTH_METHOD_T
 *           (JJ, 2002-05-24)
 * ---------------------------------------------------------------------
 */
UI32_T USERAUTH_GetRunningAuthMethod(USERAUTH_Auth_Method_T *auth_method);

/* ---------------------------------------------------------------------
 *  ROUTINE NAME  - USERAUTH_GetEnableAuthMethod
 * ---------------------------------------------------------------------
 *  FUNCTION: This function gets enable authentication method
 *  INPUT   : enable_auth_method
 *  OUTPUT  : enable_auth_method
 *  RETURN	: BOOL_T True : Get successfully, False : Get failed
 * ---------------------------------------------------------------------
 *  NOTE    : None
 */
BOOL_T USERAUTH_GetEnableAuthMethod(USERAUTH_Auth_Method_T *enable_auth_method);


/* ---------------------------------------------------------------------
 *  ROUTINE NAME  - USERAUTH_SetEnableAuthMethod
 * ---------------------------------------------------------------------
 *  FUNCTION: This function sets enable authentication method
 *  INPUT   : enable_auth_method
 *  OUTPUT  : None
 *  RETURN	: BOOL_T True : Get successfully, False : Get failed
 * ---------------------------------------------------------------------
 *  NOTE    : None
 */
BOOL_T USERAUTH_SetEnableAuthMethod(USERAUTH_Auth_Method_T *enable_auth_method);


/* ---------------------------------------------------------------------
 *  ROUTINE NAME  - USERAUTH_GetRunningEnableAuthMethod
 * ---------------------------------------------------------------------
 * PURPOSE: This function returns SYS_TYPE_GET_RUNNING_CFG_SUCCESS if the
 *          authentication method is changed.  Otherwise,
 *          SYS_TYPE_GET_RUNNING_CFG_FAIL
 *          or SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE is returned.
 * INPUT : enable_auth_method
 * OUTPUT: enable_auth_method
 * RETURN: SYS_TYPE_GET_RUNNING_CFG_SUCCESS, SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE, or
 *         SYS_TYPE_GET_RUNNING_CFG_FAIL
 * NOTES: 1. This function shall only be invoked by CLI to save the
 *           "running configuration" to local or remote files.
 *        2. Since only non-default configuration will be saved, this
 *           function shall return non-default privilege password.
 */
UI32_T USERAUTH_GetRunningEnableAuthMethod(USERAUTH_Auth_Method_T *enable_auth_method);


/* ---------------------------------------------------------------------
 *  ROUTINE NAME  - USERAUTH_GetTelnetLoginMethod
 * ---------------------------------------------------------------------
 *  FUNCTION: This function gets telnet login method
 *  INPUT   : login_method
 *  OUTPUT  : login_method
 *  RETURN	: BOOL_T True : Get successfully, False : Get failed
 * ---------------------------------------------------------------------
 *  NOTE    : None
 */
BOOL_T USERAUTH_GetTelnetLoginMethod(USERAUTH_Login_Method_T *login_method);


/* ---------------------------------------------------------------------
 *  ROUTINE NAME  - USERAUTH_SetTelnetLoginMethod
 * ---------------------------------------------------------------------
 *  FUNCTION: This function sets telnet login method
 *  INPUT   : login_method
 *  OUTPUT  : None
 *  RETURN	: BOOL_T True : Get successfully, False : Get failed
 * ---------------------------------------------------------------------
 *  NOTE    : None
 */
BOOL_T USERAUTH_SetTelnetLoginMethod(USERAUTH_Login_Method_T login_method);


/* ---------------------------------------------------------------------
 *  ROUTINE NAME  - USERAUTH_GetRunningTelnetLoginMethod
 * ---------------------------------------------------------------------
 * PURPOSE: This function returns SYS_TYPE_GET_RUNNING_CFG_SUCCESS if the
 *          telnet login method is changed.  Otherwise,
 *          SYS_TYPE_GET_RUNNING_CFG_FAIL
 *          or SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE is returned.
 * INPUT : login_method
 * OUTPUT: login_method
 * RETURN: SYS_TYPE_GET_RUNNING_CFG_SUCCESS, SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE, or
 *         SYS_TYPE_GET_RUNNING_CFG_FAIL
 * NOTES: 1. This function shall only be invoked by CLI to save the
 *           "running configuration" to local or remote files.
 *        2. Since only non-default configuration will be saved, this
 *           function shall return non-default privilege password.
 */
UI32_T USERAUTH_GetRunningTelnetLoginMethod(USERAUTH_Login_Method_T *login_method);


/* ---------------------------------------------------------------------
 *  ROUTINE NAME  - USERAUTH_GetConsolLoginMethod
 * ---------------------------------------------------------------------
 *  FUNCTION: This function gets console login method
 *  INPUT   : login_method
 *  OUTPUT  : login_method
 *  RETURN	: BOOL_T True : Get successfully, False : Get failed
 * ---------------------------------------------------------------------
 *  NOTE    : None
 */
BOOL_T USERAUTH_GetConsolLoginMethod(USERAUTH_Login_Method_T *login_method);


/* ---------------------------------------------------------------------
 *  ROUTINE NAME  - USERAUTH_SetConsoleLoginMethod
 * ---------------------------------------------------------------------
 *  FUNCTION: This function sets console login method
 *  INPUT   : login_method
 *  OUTPUT  : None
 *  RETURN	: BOOL_T True : Get successfully, False : Get failed
 * ---------------------------------------------------------------------
 *  NOTE    : None
 */
BOOL_T USERAUTH_SetConsoleLoginMethod(USERAUTH_Login_Method_T login_method);


/* ---------------------------------------------------------------------
 *  ROUTINE NAME  - USERAUTH_GetRunningConsoleLoginMethod
 * ---------------------------------------------------------------------
 * PURPOSE: This function returns SYS_TYPE_GET_RUNNING_CFG_SUCCESS if the
 *          console login method is changed.  Otherwise,
 *          SYS_TYPE_GET_RUNNING_CFG_FAIL
 *          or SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE is returned.
 * INPUT : login_method
 * OUTPUT: login_method
 * RETURN: SYS_TYPE_GET_RUNNING_CFG_SUCCESS, SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE, or
 *         SYS_TYPE_GET_RUNNING_CFG_FAIL
 * NOTES: 1. This function shall only be invoked by CLI to save the
 *           "running configuration" to local or remote files.
 *        2. Since only non-default configuration will be saved, this
 *           function shall return non-default privilege password.
 */
UI32_T USERAUTH_GetRunningConsoleLoginMethod(USERAUTH_Login_Method_T *login_method);


/* ---------------------------------------------------------------------
 *  ROUTINE NAME  - USERAUTH_GetConsoleLoginPassword
 * ---------------------------------------------------------------------
 *  FUNCTION: This function gets console login password
 *  INPUT   : password
 *  OUTPUT  : password
 *  RETURN	: BOOL_T True : Get successfully, False : Get failed
 * ---------------------------------------------------------------------
 *  NOTE    : None
 */
BOOL_T USERAUTH_GetConsoleLoginPassword(char *password);


/* ---------------------------------------------------------------------
 *  ROUTINE NAME  - USERAUTH_SetConsoleLoginPassword
 * ---------------------------------------------------------------------
 *  FUNCTION: This function sets console login password
 *  INPUT   : login_method
 *  OUTPUT  : None
 *  RETURN	: BOOL_T True : Get successfully, False : Get failed
 * ---------------------------------------------------------------------
 *  NOTE    : None
 */
BOOL_T USERAUTH_SetConsoleLoginPassword(const char *password);


/* ---------------------------------------------------------------------
 *  ROUTINE NAME  - USERAUTH_GetRunningConsoleLoginPassword
 * ---------------------------------------------------------------------
 * PURPOSE: This function returns SYS_TYPE_GET_RUNNING_CFG_SUCCESS if the
 *          console login password is changed.  Otherwise,
 *          SYS_TYPE_GET_RUNNING_CFG_FAIL
 *          or SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE is returned.
 * INPUT : login_method
 * OUTPUT: login_method
 * RETURN: SYS_TYPE_GET_RUNNING_CFG_SUCCESS, SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE, or
 *         SYS_TYPE_GET_RUNNING_CFG_FAIL
 * NOTES: 1. This function shall only be invoked by CLI to save the
 *           "running configuration" to local or remote files.
 *        2. Since only non-default configuration will be saved, this
 *           function shall return non-default privilege password.
 */
UI32_T USERAUTH_GetRunningConsoleLoginPassword(char *password);


/* ---------------------------------------------------------------------
 *  ROUTINE NAME  - USERAUTH_GetTelnetLoginPassword
 * ---------------------------------------------------------------------
 *  FUNCTION: This function gets telnet login password
 *  INPUT   : password
 *  OUTPUT  : password
 *  RETURN	: BOOL_T True : Get successfully, False : Get failed
 * ---------------------------------------------------------------------
 *  NOTE    : None
 */
BOOL_T USERAUTH_GetTelnetLoginPassword(char *password);


/* ---------------------------------------------------------------------
 *  ROUTINE NAME  - USERAUTH_SetTelnetLoginPassword
 * ---------------------------------------------------------------------
 *  FUNCTION: This function sets telnet login password
 *  INPUT   : password
 *  OUTPUT  : None
 *  RETURN	: BOOL_T True : Get successfully, False : Get failed
 * ---------------------------------------------------------------------
 *  NOTE    : None
 */
BOOL_T USERAUTH_SetTelnetLoginPassword(const char *password);


/* ---------------------------------------------------------------------
 *  ROUTINE NAME  - USERAUTH_GetRunningTelnetLoginPassword
 * ---------------------------------------------------------------------
 * PURPOSE: This function returns SYS_TYPE_GET_RUNNING_CFG_SUCCESS if the
 *          telnet login password is changed.  Otherwise,
 *          SYS_TYPE_GET_RUNNING_CFG_FAIL
 *          or SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE is returned.
 * INPUT : login_method
 * OUTPUT: login_method
 * RETURN: SYS_TYPE_GET_RUNNING_CFG_SUCCESS, SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE, or
 *         SYS_TYPE_GET_RUNNING_CFG_FAIL
 * NOTES: 1. This function shall only be invoked by CLI to save the
 *           "running configuration" to local or remote files.
 *        2. Since only non-default configuration will be saved, this
 *           function shall return non-default privilege password.
 */
UI32_T USERAUTH_GetRunningTelnetLoginPassword(char *password);

#if (SYS_CPNT_AUTHENTICATION == TRUE)

/*-------------------------------------------------------------------------
 * FUNCTION NAME:  USERAUTH_LoginAuthPassByWhichMethod
 *-------------------------------------------------------------------------
 * PURPOSE  : to get the auth-method that USERAUTH_LoginAuth() used
 * INPUT    : none
 * OUTPUT   : auth_method
 * RETURN   : TRUE - success; FALSE - fail
 * NOTES    : if the return value of USERAUTH_LoginAuth() is FALSE (fail to auth),
 *            can't get auth_method (because it didn't pass authentication)
 *-------------------------------------------------------------------------*/
BOOL_T USERAUTH_LoginAuthPassByWhichMethod(USERAUTH_Auth_Method_T *auth_method);

#endif /* SYS_CPNT_AUTHENTICATION == TRUE */



#if (SYS_CPNT_3COM_PASSWORD_RECOVERY == TRUE)
/* FUNCTION NAME:  USERAUTH_SetPasswordRecoveryActive
 * PURPOSE:
 *          This function set status of PasswordRecoveryActive.
 *
 * INPUT:
 *          BOOL_T  active  --  status of password recovery active.
 *
 * OUTPUT:
 *          none.
 *
 * RETURN:
 *          BOOL_T --  TRUE or FALSE.
 * NOTES:
 *          .
 */
BOOL_T USERAUTH_SetPasswordRecoveryActive(BOOL_T active);



/* FUNCTION NAME:  USERAUTH_GetPasswordRecoveryActive
 * PURPOSE:
 *          This function get status of PasswordRecoveryActive.
 *
 * INPUT:
 *          none.
 *
 * OUTPUT:
 *          BOOL_T  *active --  status of password recovery active.
 *
 * RETURN:
 *          BOOL_T --  TRUE or FALSE.
 * NOTES:
 *          .
 */
BOOL_T USERAUTH_GetPasswordRecoveryActive(BOOL_T *active);



/* FUNCTION NAME:  USERAUTH_DoPasswordRecovery
 * PURPOSE:
 *          This function to reset admin's password or set new admin's password.
 *
 * INPUT:
 *          none.
 *
 * OUTPUT:
 *          BOOL_T  *active --  status of password recovery active.
 *
 * RETURN:
 *          BOOL_T --  TRUE or FALSE.
 * NOTES:
 *          .
 */
BOOL_T USERAUTH_DoPasswordRecovery(UI8_T *password);



/* FUNCTION NAME:  USERAUTH_SetPasswordRecoveryStatus
 * PURPOSE:
 *          This function set status of PasswordRecoveryEnabled.
 *
 * INPUT:
 *          BOOL_T  status  --  status of password recovery enabled.
 *
 * OUTPUT:
 *          none.
 *
 * RETURN:
 *          BOOL_T --  TRUE or FALSE.
 * NOTES:
 *          .
 */
BOOL_T USERAUTH_SetPasswordRecoveryStatus(BOOL_T status);



/* FUNCTION NAME:  USERAUTH_GetPasswordRecoveryStatus
 * PURPOSE:
 *          This function get status of PasswordRecoveryEnabled.
 *
 * INPUT:
 *          none.
 *
 * OUTPUT:
 *          UI32_T  *status --  status of password recovery enabled.
 *
 * RETURN:
 *          BOOL_T --  TRUE or FALSE.
 * NOTES:
 *          .
 */
BOOL_T USERAUTH_GetPasswordRecoveryStatus(UI32_T *status);
#endif


/* FUNCTION NAME:  USERAUTH_SetAuthMethodBySessionType
 * PURPOSE:
 *          This function sets authentication method by session type.
 *
 * INPUT:
 *          USERAUTH_Auth_Method_T  *auth_method    --  auth_method
 *          UI32_T                  sess_type       --  session type
 *
 * OUTPUT:
 *          none.
 *
 * RETURN:
 *          BOOL_T  --  TRUE  -- successfully, or
 *                      FALSE -- fail
 * NOTES:
 *          .
 */
BOOL_T USERAUTH_SetAuthMethodBySessionType(USERAUTH_Auth_Method_T *auth_method, UI32_T sess_type);



/* FUNCTION NAME:  USERAUTH_GetAuthMethodBySessionType
 * PURPOSE:
 *          This function gets authentication method by session type.
 *
 * INPUT:
 *          UI32_T                  sess_type       --  session type
 *
 * OUTPUT:
 *          USERAUTH_Auth_Method_T  *auth_method    --  auth_method
 *
 * RETURN:
 *          BOOL_T  --  TRUE  -- successfully, or
 *                      FALSE -- fail
 * NOTES:
 *          .
 */
BOOL_T USERAUTH_GetAuthMethodBySessionType(USERAUTH_Auth_Method_T *auth_method, UI32_T sess_type);



/* FUNCTION NAME:  USERAUTH_GetRunningAuthMethodBySessionType
 * PURPOSE:
 *			This function returns SYS_TYPE_GET_RUNNING_CFG_SUCCESS if the
 *          authentication method is changed.  Otherwise,
 *          SYS_TYPE_GET_RUNNING_CFG_FAIL
 *          or SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE is returned.
 *
 * INPUT:
 *          UI32_T                  sess_type       --  session type
 *
 * OUTPUT:
 *          USERAUTH_Auth_Method_T  *auth_method    --  auth_method.
 *
 * RETURN:
 *          SYS_TYPE_GET_RUNNING_CFG_SUCCESS, SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE, or
 *          SYS_TYPE_GET_RUNNING_CFG_FAIL
 * NOTES:
 *          1. This function shall only be invoked by CLI to save the
 *             "running configuration" to local or remote files.
 *          2. Since only non-default configuration will be saved, this
 *             function shall return non-default structure for each field for the device.
 */
SYS_TYPE_Get_Running_Cfg_T  USERAUTH_GetRunningAuthMethodBySessionType(USERAUTH_Auth_Method_T *auth_method, UI32_T sess_type);



/* FUNCTION NAME - USERAUTH_INIT_HandleHotInsertion
 *
 * PURPOSE  : This function will initialize the port OM of the module ports
 *            when the option module is inserted.
 *
 * INPUT    : starting_port_ifindex -- the ifindex of the first module port
 *                                     inserted
 *            number_of_port        -- the number of ports on the inserted
 *                                     module
 *            use_default           -- the flag indicating the default
 *                                     configuration is used without further
 *                                     provision applied; TRUE if a new module
 *                                     different from the original one is
 *                                     inserted
 *
 * OUTPUT   : None
 *
 * RETURN   : None
 *
 * NOTE     : Only one module is inserted at a time.
 */
void USERAUTH_INIT_HandleHotInsertion(UI32_T starting_port_ifindex, UI32_T number_of_port, BOOL_T use_default);



/* FUNCTION NAME - USERAUTH_INIT_HandleHotRemoval
 *
 * PURPOSE  : This function will clear the port OM of the module ports when
 *            the option module is removed.
 *
 * INPUT    : starting_port_ifindex -- the ifindex of the first module port
 *                                     removed
 *            number_of_port        -- the number of ports on the removed
 *                                     module
 *
 * OUTPUT   : None
 *
 * RETURN   : None
 *
 * NOTE     : Only one module is removed at a time.
 */
void USERAUTH_INIT_HandleHotRemoval(UI32_T starting_port_ifindex, UI32_T number_of_port);



/* FUNCTION NAME - USERAUTH_MGR_HandleHotInsertion
 *
 * PURPOSE  : This function will initialize the port OM of the module ports
 *            when the option module is inserted.
 *
 * INPUT    : starting_port_ifindex -- the ifindex of the first module port
 *                                     inserted
 *            number_of_port        -- the number of ports on the inserted
 *                                     module
 *            use_default           -- the flag indicating the default
 *                                     configuration is used without further
 *                                     provision applied; TRUE if a new module
 *                                     different from the original one is
 *                                     inserted
 *
 * OUTPUT   : None
 *
 * RETURN   : None
 *
 * NOTE     : Only one module is inserted at a time.
 */
void USERAUTH_MGR_HandleHotInsertion(UI32_T starting_port_ifindex, UI32_T number_of_port, BOOL_T use_default);



/* FUNCTION NAME - USERAUTH_MGR_HandleHotRemoval
 *
 * PURPOSE  : This function will clear the port OM of the module ports when
 *            the option module is removed.
 *
 * INPUT    : starting_port_ifindex -- the ifindex of the first module port
 *                                     removed
 *            number_of_port        -- the number of ports on the removed
 *                                     module
 *
 * OUTPUT   : None
 *
 * RETURN   : None
 *
 * NOTE     : Only one module is removed at a time.
 */
void USERAUTH_MGR_HandleHotRemoval(UI32_T starting_port_ifindex, UI32_T number_of_port);



/* FUNCTION NAME:  USERAUTH_BackdoorFunction
 * PURPOSE:
 *          Display back door available function and accept user seletion.
 *
 * INPUT:
 *          none.
 *
 * OUTPUT:
 *          none.
 *
 * RETURN:
 *          none.
 * NOTES:
 *          .
 */
void USERAUTH_BackdoorFunction();


/* FUNCTION NAME:  USERAUTH_ProcessAuthResult
 * PURPOSE:
 *          Process authentication result from system callback.
 *
 * INPUT:
 *          param_p -- userauth parameter.
 *
 * OUTPUT:
 *          None.
 *
 * RETURN:
 *          If USERAUTH_PROCESSING is returned, .... Otherwise, send a
 *          response to orig authentication function.
 * NOTES:
 *          .
 */
USERAUTH_ReturnValue_T USERAUTH_ProcessAuthResult(
    USERAUTH_AsyncAuthParam_T *param_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME : USERAUTH_HandleIPCReqMsg
 *------------------------------------------------------------------------------
 * PURPOSE:
 *    Handle the ipc request message for csca mgr.
 * INPUT:
 *    ipcmsg_p  --  input request ipc message buffer
 *
 * OUTPUT:
 *    ipcmsg_p  --  output response ipc message buffer
 *
 * RETURN:
 *    TRUE  - There is a response need to send.
 *    FALSE - There is no response to send.
 *
 * NOTES:
 *    None.
 *------------------------------------------------------------------------------
 */
BOOL_T USERAUTH_HandleIPCReqMsg(SYSFUN_Msg_T* ipcmsg_p);

#endif
