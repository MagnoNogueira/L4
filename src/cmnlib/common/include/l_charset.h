/*
 * Copyright(C)      Accton Corporation,  2008
 *
 * Module Name: l_charset.h
 *
 * Purpose: Accton platform string verification facilities
 *
 * Notes:
 *
 * History:
 *   Date           Modifier        Reason
 * --------------- ------------     -----------------------------------
 * 2008.04.29      Steven Jiang     Linux platform ask for general string verfication APIs
 */


/* Valid Char-Set
 *      - Generic String: all printable characters
 *      - User Name: [a-zA-Z0-9@_-.] (it can support email address like names, sucha
 *                   as 'david@192.168.101.1'
 *      - Password: All printable characters include blank character.
 *                  [a-zA-Z0-9'~!@#$%^&*()[]{};':".<>/?\| ]
 *      - File Name: [a-zA-Z0-9-_.]
 *      - Path Name: [a-zA-Z0-9-_.:/\]
 */

#ifndef L_CHARSET_H
#define L_CHARSET_H

/*
 * FUNCTION:    L_CHARSET_IsValidGenericChar
 *
 * PURPOSE:     Verify if given character belongs to generic printable character
 *
 * INPUT:       c       - character
 *
 * RETURN:      0       - invalid
 *              non-0   - valid
 *
 * NOTES:       N/A
 */
int L_CHARSET_IsValidGenericChar(const char c)
{
    return 1;
}

/*
 * FUNCTION:    L_CHARSET_IsValidGenericString
 *
 * PURPOSE:     Verify if given string is a valid generic printable string
 *
 * INPUT:       s       - string to be verified
 *
 * RETURN:      0       - invalid
 *              non-0   - valid
 *
 * NOTES:       N/A
 */
int L_CHARSET_IsValidGenericString(const char *s);

/*
 * FUNCTION:    L_CHARSET_IsValidUserNameChar
 *
 * PURPOSE:     Verify if given character belongs to valid characters can be used in user name
 *
 * INPUT:       c       - character
 *
 * RETURN:      0       - invalid
 *              non-0   - valid
 *
 * NOTES:       N/A
 */
int L_CHARSET_IsValidUserNameChar(const char c);

/*
 * FUNCTION:    L_CHARSET_IsValidUserNameString
 *
 * PURPOSE:     Verify if given string is a valid user name (string)
 *
 * INPUT:       s       - string to be verified
 *
 * RETURN:      0       - invalid
 *              non-0   - valid
 *
 * NOTES:       N/A
 */
int L_CHARSET_IsValidUserNameString(const char *s);

/*
 * FUNCTION:    L_CHARSET_IsValidPasswordChar
 *
 * PURPOSE:     Verify if given character belongs to valid characters can be used in password
 *
 * INPUT:       c       - character
 *
 * RETURN:      0       - invalid
 *              non-0   - valid
 *
 * NOTES:       N/A
 */
int L_CHARSET_IsValidPasswordChar(const char c);

/*
 * FUNCTION:    L_CHARSET_IsValidPasswordString
 *
 * PURPOSE:     Verify if given string is a valid password (string)
 *
 * INPUT:       s       - string to be verified
 *
 * RETURN:      0       - invalid
 *              non-0   - valid
 *
 * NOTES:       N/A
 */
int L_CHARSET_IsValidPasswordString(const char *s);

/*
 * FUNCTION:    L_CHARSET_IsValidFileNameChar
 *
 * PURPOSE:     Verify if given character belongs to valid characters can be used in file name
 *
 * INPUT:       c       - character
 *
 * RETURN:      0       - invalid
 *              non-0   - valid
 *
 * NOTES:       N/A
 */
int L_CHARSET_IsValidFileNameChar(const char c);

/*
 * FUNCTION:    L_CHARSET_IsValidFileNameString
 *
 * PURPOSE:     Verify if given string is a valid file name (string)
 *
 * INPUT:       s       - string to be verified
 *
 * RETURN:      0       - invalid
 *              non-0   - valid
 *
 * NOTES:       N/A
 */
int L_CHARSET_IsValidFileNameString(const char *s);

/*
 * FUNCTION:    L_CHARSET_IsValidPathNameChar
 *
 * PURPOSE:     Verify if given character belongs to valid characters can be used in path name
 *
 * INPUT:       c       - character
 *
 * RETURN:      0       - invalid
 *              non-0   - valid
 *
 * NOTES:       N/A
 */
int L_CHARSET_IsValidPathNameChar(const char c);

/*
 * FUNCTION:    L_CHARSET_IsValidPathNameString
 *
 * PURPOSE:     Verify if given string is a valid path name (string)
 *
 * INPUT:       s       - string to be verified
 *
 * RETURN:      0       - invalid
 *              non-0   - valid
 *
 * NOTES:       N/A
 */
int L_CHARSET_IsValidPathNameString(const char *s);


#endif /* L_CHARSET_H */
