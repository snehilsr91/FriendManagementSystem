#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_USERS 100
#define MAX_FRIENDS 10
#define MAX_REQUESTS 10
#define ID_LENGTH 20

// User structure
struct User {
    char userID[ID_LENGTH];                  // Unique User ID (string)
    char displayName[50];                    // Display Name
    char password[50];                       // Password
    char friends[MAX_FRIENDS][ID_LENGTH];    // Friends (store UserIDs)
    int friendCount;

    char requests[MAX_REQUESTS][ID_LENGTH];  // Incoming friend requests
    int requestCount;

    char requested[MAX_REQUESTS][ID_LENGTH]; // Sent friend requests
    int requestedCount;
};

// Array to store users
struct User users[MAX_USERS];
int userCount = 0;

// Helper function to find user index by ID
int findUserIndexByID(const char *userID) {
    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].userID, userID) == 0) return i;
    }
    return -1; // Not found
}

// Function to check if a userID exists
bool isUserIDTaken(const char *userID) {
    return findUserIndexByID(userID) != -1;
}

// Function to create a new user
void createNewUser() {
    char userID[ID_LENGTH], displayName[50], password[50];

    printf("Enter a new UserID: ");
    scanf("%s", userID);

    if (isUserIDTaken(userID)) {
        printf("Error: UserID '%s' already exists.\n", userID);
        return;
    }

    printf("Enter Display Name: ");
    scanf("%s", displayName);

    printf("Enter Password: ");
    scanf("%s", password);

    strcpy(users[userCount].userID, userID);
    strcpy(users[userCount].displayName, displayName);
    strcpy(users[userCount].password, password);
    users[userCount].friendCount = 0;
    users[userCount].requestCount = 0;
    users[userCount].requestedCount = 0;

    userCount++;
    printf("User created successfully!\n");
}

// Function to display total users
void displayTotalUsers() {
    printf("Total number of users: %d\n", userCount);
}

// Function to send a friend request
void addFriend(int currentUserIndex) {
    char friendID[ID_LENGTH];
    printf("Enter UserID of the friend you want to add: ");
    scanf("%s", friendID);

    if (strcmp(friendID, users[currentUserIndex].userID) == 0) {
        printf("You cannot send a friend request to yourself!\n");
        return;
    }

    int friendIndex = findUserIndexByID(friendID);
    if (friendIndex == -1) {
        printf("User not found!\n");
        return;
    }

    // Check if already friends
    for (int i = 0; i < users[currentUserIndex].friendCount; i++) {
        if (strcmp(users[currentUserIndex].friends[i], friendID) == 0) {
            printf("You are already friends!\n");
            return;
        }
    }

    // Add to requests and requested arrays
    strcpy(users[currentUserIndex].requested[users[currentUserIndex].requestedCount++], friendID);
    strcpy(users[friendIndex].requests[users[friendIndex].requestCount++], users[currentUserIndex].userID);

    printf("Friend request sent to %s!\n", users[friendIndex].displayName);
}

// Function to display friends
void displayFriends(int currentUserIndex) {
    printf("\nYour Friends:\n");
    if (users[currentUserIndex].friendCount == 0) {
        printf("No friends yet.\n");
        return;
    }

    for (int i = 0; i < users[currentUserIndex].friendCount; i++) {
        int friendIndex = findUserIndexByID(users[currentUserIndex].friends[i]);
        printf("- UserID: %s, Display Name: %s\n", users[friendIndex].userID, users[friendIndex].displayName);
    }
}

// Function to remove a friend
void removeFriend(int currentUserIndex) {
    char friendID[ID_LENGTH];
    printf("Enter UserID of the friend to remove: ");
    scanf("%s", friendID);

    int friendIndex = findUserIndexByID(friendID);
    if (friendIndex == -1) {
        printf("User not found!\n");
        return;
    }

    bool found = false;
    for (int i = 0; i < users[currentUserIndex].friendCount; i++) {
        if (strcmp(users[currentUserIndex].friends[i], friendID) == 0) {
            found = true;
            for (int j = i; j < users[currentUserIndex].friendCount - 1; j++) {
                strcpy(users[currentUserIndex].friends[j], users[currentUserIndex].friends[j + 1]);
            }
            users[currentUserIndex].friendCount--;
            break;
        }
    }

    if (found) {
        for (int i = 0; i < users[friendIndex].friendCount; i++) {
            if (strcmp(users[friendIndex].friends[i], users[currentUserIndex].userID) == 0) {
                for (int j = i; j < users[friendIndex].friendCount - 1; j++) {
                    strcpy(users[friendIndex].friends[j], users[friendIndex].friends[j + 1]);
                }
                users[friendIndex].friendCount--;
                break;
            }
        }
        printf("Friend removed successfully.\n");
    } else {
        printf("User is not in your friend list.\n");
    }
}

// Function to suggest friends
void suggestFriends(int currentUserIndex) {
    printf("\nFriend Suggestions:\n");
    bool suggested[MAX_USERS] = {false};

    for (int i = 0; i < users[currentUserIndex].friendCount; i++) {
        int friendIndex = findUserIndexByID(users[currentUserIndex].friends[i]);
        for (int j = 0; j < users[friendIndex].friendCount; j++) {
            int suggestionIndex = findUserIndexByID(users[friendIndex].friends[j]);
            if (suggestionIndex != -1 && strcmp(users[suggestionIndex].userID, users[currentUserIndex].userID) != 0) {
                if (!suggested[suggestionIndex]) {
                    suggested[suggestionIndex] = true;
                    printf("- UserID: %s, Display Name: %s\n", users[suggestionIndex].userID, users[suggestionIndex].displayName);
                }
            }
        }
    }
}

// Function to check incoming friend requests
void checkFriendRequests(int currentUserIndex) {
    int choice;
    if (users[currentUserIndex].requestCount == 0) {
        printf("No incoming friend requests.\n");
        return;
    }

    printf("Incoming Friend Requests:\n");
    for (int i = 0; i < users[currentUserIndex].requestCount; i++) {
        printf("- UserID: %s\n", users[currentUserIndex].requests[i]);
    }

    do {
        printf("\nEnter the UserID to accept/reject or 0 to go back: ");
        char userID[ID_LENGTH];
        scanf("%s", userID);
        
        if (strcmp(userID, "0") == 0) break;
        
        int requestIndex = -1;
        for (int i = 0; i < users[currentUserIndex].requestCount; i++) {
            if (strcmp(users[currentUserIndex].requests[i], userID) == 0) {
                requestIndex = i;
                break;
            }
        }

        if (requestIndex == -1) {
            printf("Invalid UserID. No request found.\n");
            continue;
        }

        int friendIndex = findUserIndexByID(userID);
        if (friendIndex == -1) {
            printf("User not found.\n");
            continue;
        }

        // Accept friend request
        printf("Do you want to accept the friend request from %s? (1 for Yes, 0 for No): ", userID);
        scanf("%d", &choice);
        if (choice == 1) {
            strcpy(users[currentUserIndex].friends[users[currentUserIndex].friendCount++], userID);
            strcpy(users[friendIndex].friends[users[friendIndex].friendCount++], users[currentUserIndex].userID);

            // Remove the accepted request
            for (int i = requestIndex; i < users[currentUserIndex].requestCount - 1; i++) {
                strcpy(users[currentUserIndex].requests[i], users[currentUserIndex].requests[i + 1]);
            }
            users[currentUserIndex].requestCount--;

            printf("Friend request accepted!\n");
        } else {
            // Reject friend request
            for (int i = requestIndex; i < users[currentUserIndex].requestCount - 1; i++) {
                strcpy(users[currentUserIndex].requests[i], users[currentUserIndex].requests[i + 1]);
            }
            users[currentUserIndex].requestCount--;
            printf("Friend request rejected.\n");
        }
    } while (1);
}

// Function to check requested users
void checkRequestedUsers(int currentUserIndex) {
    if (users[currentUserIndex].requestedCount == 0) {
        printf("No friend requests sent.\n");
        return;
    }

    printf("Users you have sent friend requests to:\n");
    for (int i = 0; i < users[currentUserIndex].requestedCount; i++) {
        printf("- UserID: %s\n", users[currentUserIndex].requested[i]);
    }
}

// Inner menu
void loggedInMenu(int currentUserIndex) {
    int choice;
    do {
        printf("\n--- Inner Menu ---\n");
        printf("1. Add a friend\n");
        printf("2. Remove a friend\n");
        printf("3. Display friends\n");
        printf("4. Suggest friends\n");
        printf("5. Check friend requests\n");
        printf("6. Check requested users\n");
        printf("7. Logout\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: addFriend(currentUserIndex); break;
            case 2: removeFriend(currentUserIndex); break;
            case 3: displayFriends(currentUserIndex); break;
            case 4: suggestFriends(currentUserIndex); break;
            case 5: checkFriendRequests(currentUserIndex); break;
            case 6: checkRequestedUsers(currentUserIndex); break;
            case 7: printf("Logged out successfully.\n"); break;
            default: printf("Invalid choice!\n");
        }
    } while (choice != 7);
}

// Function to login
void loginUser() {
    char userID[ID_LENGTH], password[50];
    printf("Enter UserID: ");
    scanf("%s", userID);
    printf("Enter Password: ");
    scanf("%s", password);

    int userIndex = findUserIndexByID(userID);
    if (userIndex == -1 || strcmp(users[userIndex].password, password) != 0) {
        printf("Invalid UserID or Password!\n");
        return;
    }

    printf("Login successful. Welcome, %s!\n", users[userIndex].displayName);
    loggedInMenu(userIndex);
}

// Main function
int main() {
    int choice;
    do {
        printf("\n--- Outer Menu ---\n");
        printf("1. Check total users\n");
        printf("2. Create new user\n");
        printf("3. Login to an account\n");
        printf("4. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: displayTotalUsers(); break;
            case 2: createNewUser(); break;
            case 3: loginUser(); break;
            case 4: printf("Exiting program. Goodbye!\n"); break;
            default: printf("Invalid choice!\n");
        }
    } while (choice != 4);
    return 0;
}