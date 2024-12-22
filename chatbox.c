#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_USERS 100
#define MAX_FRIENDS 10
#define MAX_REQUESTS 10
#define MAX_CONVOS 100
#define MAX_MESSAGES 100
#define ID_LENGTH 20
#define USER_FILE "users.db"

// Message structure
struct Message {
    char fromUserID[ID_LENGTH]; // Sender's UserID
    char messageText[255];      // Message text
};

// Conversation structure
struct Conversation {
    struct Message messages[MAX_MESSAGES]; // Array of messages
    int messageCount; // Number of messages in this conversation
    char user1[ID_LENGTH]; // UserID of the first participant
    char user2[ID_LENGTH]; // UserID of the second participant
};

// Global variables
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

struct User users[MAX_USERS];
int userCount = 0;

struct Conversation conversations[MAX_CONVOS];
int conversationCount = 0;  // Keep track of the number of conversations

// Function to save users data to file
void saveUsersToFile() {
    FILE *file = fopen("users.db", "wb");
    if (!file) {
        printf("Unable to open file for saving.\n");
        return;
    }

    // Save users data
    fwrite(&userCount, sizeof(int), 1, file);
    fwrite(users, sizeof(struct User), userCount, file);

    // Save conversations data
    fwrite(&conversationCount, sizeof(int), 1, file);
    for (int i = 0; i < conversationCount; i++) {
        fwrite(&conversations[i], sizeof(struct Conversation), 1, file);  // Save each conversation
    }

    fclose(file);
}

// Function to load users data from file
void loadFiles() {
    FILE *file = fopen("users.db", "rb");
    if (!file) {
        printf("Unable to open file for loading.\n");
        return;
    }

    // Load users data
    fread(&userCount, sizeof(int), 1, file);
    fread(users, sizeof(struct User), userCount, file);

    // Load conversations data
    fread(&conversationCount, sizeof(int), 1, file);
    for (int i = 0; i < conversationCount; i++) {
        fread(&conversations[i], sizeof(struct Conversation), 1, file);  // Load each conversation
    }

    fclose(file);
}

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

// Function to create a new user and save data to file
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

    saveUsersToFile();  // Save updated user data to file

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

    saveUsersToFile();  // Save after request

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
        saveUsersToFile();  // Save after removal
        printf("Friend removed successfully.\n");
    } else {
        printf("User is not in your friend list.\n");
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

        printf("Accept this friend request? (1 for Yes, 0 for No): ");
        scanf("%d", &choice);

        if (choice == 1) {
            // Add friend to both users
            int friendIndex = findUserIndexByID(userID);
            strcpy(users[currentUserIndex].friends[users[currentUserIndex].friendCount++], userID);
            strcpy(users[friendIndex].friends[users[friendIndex].friendCount++], users[currentUserIndex].userID);

            // Remove from requests
            for (int i = requestIndex; i < users[currentUserIndex].requestCount - 1; i++) {
                strcpy(users[currentUserIndex].requests[i], users[currentUserIndex].requests[i + 1]);
            }
            users[currentUserIndex].requestCount--;

            printf("You are now friends with %s.\n", userID);
            saveUsersToFile();  // Save after accepting
        } else {
            printf("Friend request rejected.\n");
        }

    } while (1);
}

// Function to implement friend suggestions
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

// Chatbox function
void chatbox(int currentUserIndex) {
    char friendID[ID_LENGTH];
    printf("\nEnter the UserID of the friend you want to chat with: ");
    scanf("%s", friendID);

    int friendIndex = findUserIndexByID(friendID);
    if (friendIndex == -1) {
        printf("Friend not found!\n");
        return;
    }
    if(friendIndex==currentUserIndex){
        printf("You cannot send messages to yourself.");
        return;
    }
    bool isFriend = false;
    for (int i = 0; i < users[currentUserIndex].friendCount; i++) {
        if (strcmp(users[currentUserIndex].friends[i], friendID) == 0) {
            isFriend = true;
            break;
        }
    }

    if (!isFriend) {
        printf("You are not friends with %s.\n", friendID);
        return;
    }

    // Check if conversation already exists between the two users
    int conversationIndex = -1;
    for (int i = 0; i < conversationCount; i++) {
        if ((strcmp(conversations[i].user1, users[currentUserIndex].userID) == 0 && strcmp(conversations[i].user2, friendID) == 0) ||
            (strcmp(conversations[i].user1, friendID) == 0 && strcmp(conversations[i].user2, users[currentUserIndex].userID) == 0)) {
            conversationIndex = i;
            break;
        }
    }

    // If no conversation exists, create a new one if there's space
    if (conversationIndex == -1) {
        if (conversationCount >= MAX_CONVOS) {
            printf("Server Conversations Limit Reached\n");
            return;
        }

        // Assign the next available conversation index
        conversationIndex = conversationCount;
        conversationCount++;

        // Create a new conversation between the two users
        strcpy(conversations[conversationIndex].user1, users[currentUserIndex].userID);
        strcpy(conversations[conversationIndex].user2, friendID);
        conversations[conversationIndex].messageCount = 0;  // No messages yet
    }

    // Send new messages
    char messageText[255];
    int choice;
    do {
        // Display chat history
        printf("\n--- Chat History with %s ---\n", users[friendIndex].displayName);
        for (int i = 0; i < conversations[conversationIndex].messageCount; i++) {
            // Display messages one by one
            const char *senderUserID = conversations[conversationIndex].messages[i].fromUserID;
            const char *message = conversations[conversationIndex].messages[i].messageText;

            // Find the display name of the sender
            int senderIndex = findUserIndexByID(senderUserID);
            if (senderIndex != -1) {
                printf("%s: %s\n", users[senderIndex].displayName, message);
            }
        }
        printf("\nDo you want to send a message? (1 for Yes, 0 to go back): ");
        scanf("%d", &choice);
        if(!choice)
        continue;
        printf("\nEnter your message (max 255 chars): ");
        getchar();  // To consume any leftover newline
        fgets(messageText, 255, stdin);

        messageText[strcspn(messageText, "\n")] = '\0';  // Remove the newline character from fgets

        // Add the message to the conversation
        strcpy(conversations[conversationIndex].messages[conversations[conversationIndex].messageCount].messageText, messageText);
        strcpy(conversations[conversationIndex].messages[conversations[conversationIndex].messageCount].fromUserID, users[currentUserIndex].userID);
        conversations[conversationIndex].messageCount++;

        saveUsersToFile();  // Save after sending a message

        printf("Message sent!\n");
    } while (choice == 1);
}

// Logged-In Menu
void loggedInMenu(int currentUserIndex) {
    int choice;
    do {
        printf("\n--- Logged-in Menu ---\n");
        printf("1. View friends\n");
        printf("2. Send friend request\n");
        printf("3. Check friend requests\n");
        printf("4. Suggest friends\n");
        printf("5. Remove friend\n");
        printf("6. Chatbox\n");
        printf("7. Logout\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                displayFriends(currentUserIndex);
                break;
            case 2:
                addFriend(currentUserIndex);
                break;
            case 3:
                checkFriendRequests(currentUserIndex);
                break;
            case 4:
                suggestFriends(currentUserIndex);
                break;
            case 5:
                removeFriend(currentUserIndex);
                break;
            case 6:
                chatbox(currentUserIndex);  // Added chatbox functionality here
                break;
            case 7:
                printf("Logging out...\n");
                break;
            default:
                printf("Invalid choice. Try again.\n");
        }
    } while (choice != 7);
}

// Function to log in
void loginUser() {
    loadFiles();  // Load user data from file

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
    loggedInMenu(userIndex);  // Call the menu
}

int main() {
    loadFiles();  // Load users data from file

    int choice;
    do {
        printf("\n--- Main Menu ---\n");
        printf("1. Create new user\n");
        printf("2. Display total users\n");
        printf("3. Login\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                createNewUser();
                break;
            case 2:
                displayTotalUsers();
                break;
            case 3:
                loginUser();
                break;
            case 4:
                printf("Exiting program...\n");
                break;
            default:
                printf("Invalid choice. Try again.\n");
        }
    } while (choice != 4);

    return 0;
}
