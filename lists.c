#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "lists.h"

/* Add a group with name group_name to the group_list referred to by 
* group_list_ptr. The groups are ordered by the time that the group was 
* added to the list with new groups added to the end of the list.
*
* Returns 0 on success and -1 if a group with this name already exists.
*
* (I.e, allocate and initialize a Group struct, and insert it
* into the group_list. Note that the head of the group list might change
* which is why the first argument is a double pointer.) 
*/
int add_group(Group **group_list_ptr, const char *group_name) {
    // create group name, if error occur, return -1
    char *name;
    name = malloc(strlen(group_name)+1);
    if (name == NULL) {
	perror("error: malloc\n");
	if (errno == ENOMEM )
	    exit (1);
    }
    strcpy(name, group_name);

    // create a new group, if error occur, return -1
    Group *group;
    group = malloc (sizeof(Group));
    if (group == NULL) {
	perror("error: malloc\n");
	if (errno == ENOMEM )
	    exit (1);
    }
    group->name = name;
    group->users = NULL;
    group->next = NULL;
    
    // get the first group from the group_list_ptr
    Group *curr_group = *group_list_ptr;
	
    // if there is no groups at all, let group be the first element
    if (curr_group == NULL) {
        *group_list_ptr = group;
        return 0;
    }
    // if there is at least one group
    while (curr_group != NULL) {
        // if the group name is already existed, return -1
        if (strcmp(curr_group->name, name) == 0) {
            free (group);
            free (name);
            return -1;
        }
	// if we found the last elemnet in the list, add 'group' after it
        if (curr_group->next == NULL) {
            curr_group->next = group;
            break;
        }
        curr_group = curr_group->next;
    }
    return 0;
}

/* Print to standard output the names of all groups in group_list, one name
*  per line. Output is in the same order as group_list.
*/
void list_groups(Group *group_list) {
    // get the first group from the group_list
    Group *curr_group = group_list;
    
    // loop through each group in the list and print out its name
    while (curr_group != NULL) {
        printf("%s\n", curr_group->name);
        curr_group = curr_group->next;
    }
}

/* Search the list of groups for a group with matching group_name
* If group_name is not found, return NULL, otherwise return a pointer to the 
* matching group list node.
*/
Group *find_group(Group *group_list, const char *group_name) {
    // get the first group from the group_list
    Group *curr_group = group_list;
	
    // loop through each group
    while (curr_group != NULL) {
	// if we found the group with the given group_name, return it
        if (strcmp(curr_group->name, group_name) == 0)
            return curr_group;
        curr_group = curr_group->next;
    }
    // otherwise, return NULL
    return NULL;
}

/* Add a new user with the specified user name to the specified group. Return zero
* on success and -1 if the group already has a user with that name.
* (allocate and initialize a User data structure and insert it into the
* appropriate group list)
*/
int add_user(Group *group, const char *user_name) {
    // check whether the user name is already exited using our helper fuction
    User *prev_user;
    prev_user = find_prev_user(group, user_name);
    // if the user name do exist, return -1
    if (prev_user != NULL)
        return -1;
    
    // create a user name
    char *name;
    name = malloc(strlen(user_name)+1);
    if (name == NULL) {
	perror("error: malloc\n");
	if (errno == ENOMEM )
	    exit (1);
    }
    strcpy(name, user_name);

    // create a new user
    User *user;
    user = malloc(sizeof(User));
    if (user == NULL) {
	perror("error: malloc\n");
	if (errno == ENOMEM )
	    exit (1);
    }
    user->name = name;
    user->balance = 0;

    // put the new user in front since he will have a minimum balance--0;
    user->next = group->users;
    group->users = user;
    return 0;
}

/* Remove the user with matching user and group name and
* remove all her transactions from the transaction list. 
* Return 0 on success, and -1 if no matching user exists.
* Remember to free memory no longer needed.
* (Wait on implementing the removal of the user's transactions until you 
* get to Part III below, when you will implement transactions.)
*/
int remove_user(Group *group, const char *user_name) {
    // intialize two pointers
    User *curr_user;
    User *prev_user;

    // assign prev_user by using helper function
    prev_user = find_prev_user(group, user_name);
    
    // if there is a matched user, prev_user won't be NULL
    if (prev_user != NULL) {
        // if the user is at the first of the list
        // prev_use and curr_user are overlap
	// we can detect this by checking the user_name
        if (strcmp(prev_user->name, user_name) == 0) {
            curr_user = prev_user;
            group->users = curr_user->next;
        // if the user is not the first one
        } else {
            curr_user = prev_user->next;
            prev_user->next = curr_user->next;
        }
        // delete the tansactions and free memory
        remove_xct(group, curr_user->name);
        curr_user->next = NULL;
        free (curr_user->name);
        free (curr_user);
        return 0;
    }
    // otherwise, there is no matching user, return -1
    return -1;
}

/* Print to standard output the names of all the users in group, one
* per line, and in the order that users are stored in the list, namely 
* lowest payer first.
*/
void list_users(Group *group) {
    // get the fist user fron the group
    User *curr_user = group->users;
    
    // access each user in the linked list and print his name
    while (curr_user != NULL) {
        printf("%s\n", curr_user->name);
        curr_user = curr_user->next;
    }
}

/* Print to standard output the balance of the specified user. Return 0
* on success, or -1 if the user with the given name is not in the group.
*/
int user_balance(Group *group, const char *user_name) {
    // we using helper fuction to find the prev_user first
    User *prev_user;
    prev_user = find_prev_user(group, user_name);
    // if there is no matched user
    if (prev_user == NULL)
        return -1;
    // if the target user overlap with prev_user (i.e. matched user is the first user)
    else if (strcmp(prev_user->name, user_name) == 0)
        printf("%f\n", (double)prev_user->balance);
    // if not overlap (i.e. target user is not the first user)
    else
        printf("%f\n", (double)(prev_user->next)->balance);
    return 0;
}

/* Print to standard output the name of the user who has paid the least 
* If there are several users with equal least amounts, all names are output. 
* Returns 0 on success, and -1 if the list of users is empty.
* (This should be easy, since your list is sorted by balance). 
*/
int under_paid(Group *group) {
    // get the first user from the group
    User *curr_user = group->users;
    // if the list of users is empty, return 1
    if (curr_user == NULL)
        return -1;
    
    // if not empty, get the user's balance
    int least_balance = curr_user->balance;
    
    // print all users' name if the user have the same least balance
    while (curr_user != NULL) {
        if (curr_user->balance == least_balance)
            printf("%s\n", curr_user->name);
        else
            break;
        curr_user = curr_user->next;
    }
    return 0;
}

/* Return a pointer to the user prior to the one in group with user_name. If 
* the matching user is the first in the list (i.e. there is no prior user in 
* the list), return a pointer to the matching user itself. If no matching user 
* exists, return NULL. 
*
* The reason for returning the prior user is that returning the matching user 
* itself does not allow us to change the user that occurs before the
* matching user, and some of the functions you will implement require that
* we be able to do this.
*/
User *find_prev_user(Group *group, const char *user_name) {
    User *prev = group->users;
    User *curr = prev;
    
    // if the the list is empty
    if (prev == NULL)
        return NULL;
    
    // if the matching is the first in the list, return itself
    if (strcmp(curr->name, user_name) == 0)
        return prev;
    // otherwise, assign curr to be the second user
    else
        curr = prev->next;
    
    // loop throuth the list, if we found matched user, return the previous one
    while (curr != NULL) {
        if (strcmp(curr->name, user_name) == 0)
            return prev;
        curr = curr->next;
        prev = prev->next;
    }
    // if not found, return NULL
    return NULL;
}

/* Add the transaction represented by user_name and amount to the appropriate 
* transaction list, and update the balances of the corresponding user and group. 
* Note that updating a user's balance might require the user to be moved to a
* different position in the list to keep the list in sorted order. Returns 0 on
* success, and -1 if the specified user does not exist.
*/
int add_xct(Group *group, const char *user_name, double amount) {
    // check whether the user is existed in the group
    User *prev;
    User *curr;
    prev = find_prev_user(group, user_name);
    if (prev == NULL)
        return -1;
    
    // check whether the user is at the first of the list
    if (strcmp(prev->name, user_name) == 0)
        curr = prev;
    else
        curr = prev->next;
    
    // create a transaction
    Xct *xct = malloc(sizeof(Xct));
    if (xct == NULL) {
	perror("error: malloc\n");
	if (errno == ENOMEM )
	    exit (1);
    }
    xct->name = curr->name;
    xct->amount = amount;
    xct->next = group->xcts;
    group->xcts = xct;
    
    // upade balance and move the user based on his new balance
    curr->balance += amount;

    // the user will be put between temp and temp->next
    User *temp = curr->next;
    
    // if curr is the last user in the list, position unchanged
    if (temp == NULL)
        return 0;
    // otherwise, move it to the correct position
    while ((temp->next != NULL) && (curr->balance >= (temp->next)->balance)) {
        temp = temp->next;
    }

    // move curr to the position between temp and temp->next
    // and there are 2 cases:
    // case1: curr is the first element
    if (curr == prev)
        group->users = curr->next;
    // case2: curr is not the first element
    else
        prev->next = curr->next;
    curr->next = temp->next;
    temp->next = curr;
    return 0;
}

/* Print to standard output the num_xct most recent transactions for the 
* specified group (or fewer transactions if there are less than num_xct 
* transactions posted for this group). The output should have one line per 
* transaction that prints the name and the amount of the transaction. If 
* there are no transactions, this function will print nothing.
*/
void recent_xct(Group *group, long nu_xct) {
    Xct *curr_xct = group->xcts;
    int i = 0;
    // print xct based on two condition: 1. length of the list ; 2. input nu_xct
    while ((curr_xct != NULL) && (i < nu_xct)) {
        printf("%s: %f\n", curr_xct->name, curr_xct->amount);
        curr_xct = curr_xct->next;
        i += 1;
    }
}

/* Remove all transactions that belong to the user_name from the group's 
* transaction list. This helper function should be called by remove_user. 
* If there are no transactions for this user, the function should do nothing.
* Remember to free memory no longer needed.
*/
void remove_xct(Group *group, const char *user_name) {
    Xct *curr_xct = group->xcts;
    Xct *prev_xct = curr_xct;
    
    while (curr_xct != NULL) {
	// if the xct that matched the user_name, delete xct
        if (strcmp(curr_xct->name, user_name) == 0) {
            // if curr is the first xct
            if (prev_xct == curr_xct) {
                group->xcts = curr_xct->next;
                prev_xct = curr_xct->next;
                curr_xct->next = NULL;
                free (curr_xct);
                curr_xct = prev_xct;
                }
            // if curr is not the first sct
            else {
                prev_xct->next = curr_xct->next;
                curr_xct->next = NULL;
                free (curr_xct);
                curr_xct = prev_xct->next;
            }
        // if the xct NOT match the user_name, update prev_xct and curr_xct
	} else {
            // if curr and prev not overlap, update prev
            if (prev_xct != curr_xct)
                prev_xct = curr_xct;
            curr_xct = curr_xct->next;
        }
    }
}
