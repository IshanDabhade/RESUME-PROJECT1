
// using avl trees 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define SIZE 30
typedef unsigned short HASHTYPE;

typedef struct Audiobook
{
    struct Audiobook *left, *right;
    HASHTYPE id;
    int height;
    char title[SIZE];
    char author[SIZE];
    char narrator[SIZE];
    char genre[SIZE];
    float min;
    float rating;
    // extra details
    float total_listening_time, count_rating, sum_rating;
} Audiobook;

typedef struct user
{
    struct user *left, *right;
    int height;
    HASHTYPE uid;
    char name[SIZE];
    char mail[SIZE];
    // char pref_genre[5][SIZE], pref_author[5][SIZE], pref_narrator[5][SIZE];
    struct mybook *mybooks;
} user;

typedef struct mybook
{
    struct mybook *left, *right;
    int height;
    HASHTYPE id;
    char title[SIZE];
    char author[SIZE];
    char narrator[SIZE];
    char genre[SIZE];
    float min;
    // extra details
    float uRating, percent;
} mybook;

Audiobook *Library=NULL;
user *uList=NULL;
bool isLogged = false;
HASHTYPE loggedID = 0;

HASHTYPE HASH(unsigned char *str)
{ /*
1.For generating unique userID
2. Also helps in checking if mail is unique
*/
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

// void FILEREADER()

void FILEREADER() 
{
    // READING AUDIOBOOKS
    FILE* file = fopen("DSPD3/BOOKS.dat", "rb");
    Audiobook* book = NULL;
    Audiobook temp;

    while(fread(&temp,sizeof(Audiobook),1,file)==1)
    { 
        int flag=1;
        int *f_ptr=&flag;
        book= insert(book, temp.id, temp.title, temp.author, temp.narrator, temp.genre, temp.min , temp.rating, temp.total_listening_time, temp.count_rating, temp.sum_rating, &f_ptr);
        if(*f_ptr==0)
            printf("audiobook cant be added, already there in the library!!");
    }
    fclose(file);
    Library = book ;

    // READING USERS
    file = fopen("DSPD3/USERS.dat", "rb");
    user* userlist = NULL;
    user node;

    while(fread(&node,sizeof(user),1,file)==1)
    {
        int flag=1;
        int *f_ptr=&flag;
        userlist=uinsert(userlist, node.uid, node.name, node.mail, &f_ptr);
        if(*f_ptr==0)
            printf("duplicate user entry not allowed, cant add user once again !!");
    }
    fclose(file);
    uList = userlist;

    // READING USER LIBRARIES 

    user *p3=uList;
    user_traversal(p3);
}

void *user_traversal(user *u )
{
    if(u)
    {
        char buf[SIZE];
        FILE *f = fopen(buf, "rb");
        mybook* books=NULL;
        mybook node;

        short id=u->uid;
        /*start reading*/
        snprintf(buf, sizeof(buf), "DSPD3/%hu.dat", id);
        while(fread(&node,sizeof(mybook),1,f)==1)
        {
            int flag=1;
            int *f_ptr=&flag;       
            books = mylib_insert(books, node.id, node.min, node.title, node.author, node.narrator, node.genre, node.percent, node.uRating ,&f_ptr);
            if(*f_ptr==0)
                printf("audiobook cant be added, already there in the users' library!!");
        }
        
        fclose(f);
        /*all books of current read*/

        /*now save collected books to users mybooks collection*/
        u->mybooks=books;
        user_traversal(u->left);
        user_traversal(u->right);
    }
}


// Function to create a new AVL node
Audiobook *create_book_node(HASHTYPE id, char title[], char author[], char narrator[], char genre[], float min, float r, float t, float count, float sum)
{
    // Audiobook *root = Library;
    Audiobook *root = (Audiobook *)malloc(sizeof(Audiobook));
    root->id = id;
    root->height=1; // h=1
    strcpy(root->author, author);
    strcpy(root->title, title);
    strcpy(root->narrator, narrator);
    strcpy(root->genre, genre);
    root->min = min;
    root->rating=r;
    root->total_listening_time = t;
    root->count_rating=count;
    root->sum_rating=sum;
    root->left = NULL;
    root->right = NULL;
    
    return root;
}

user *create_user_node(HASHTYPE id, char name[], char mail[])
{
    // user *root  = uList;
    user *root = (user *)malloc(sizeof(user));
    root->uid = id;
    root->height=1;
    root->mybooks = NULL;
    root->left = NULL;
    root->right=NULL ;
    strcpy(root->name, name);
    strcpy(root->mail, mail);

    return root;
}

mybook *create_mylib_node(HASHTYPE id, float min, char tit[], char aut[], char nar[], char gen[], float p, float r)
{
    mybook *root = (mybook *)malloc(sizeof(mybook));
    root->id = id;
    root->height=1;
    root->min = min;
    root->percent = p;
    root->uRating = r;
    root->left  = NULL;
    root->right=NULL;
    strcpy(root->author, aut);
    strcpy(root->title, tit);
    strcpy(root->narrator, nar);
    strcpy(root->genre, gen);

    return root ;
}


// Function to get maximum of two integers
int max(int a, int b) 
{
    return (a > b) ? a : b;
}

// Function to get the height of a node
int height( Audiobook* node) 
{
    if (node == NULL)
        return 0;
    return node->height; 
}

int uheight(user *node)
{
    if (node == NULL)
        return 0;
    return node->height; 
}

int mylib_height(mybook *node)
{
    if (node == NULL)
        return 0;
    return node->height; 
}

// Function to right rotate subtree rooted with y
Audiobook *rightRotate( Audiobook *y) 
{
     Audiobook*x = y->left;
     Audiobook*T2 = x->right;

    // Perform rotation
    x->right = y;
    y->left = T2;

    // Update heights
    y->height = max(height(y->left), height(y->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;

    // Return new root
    return x;
}

user *urightRotate(user *y)
{
    user *x = y->left;
    user *T2 = x->right;

    // Perform rotation
    x->right = y;
    y->left = T2;

    // Update heights
    y->height = max(uheight(y->left), uheight(y->right)) + 1;
    x->height = max(uheight(x->left), uheight(x->right)) + 1;

    // Return new root
    return x;
}

mybook *mylib_rightRotate(mybook *y)
{
    mybook *x = y->left;
    mybook *T2 = x->right;

    // Perform rotation
    x->right = y;
    y->left = T2;

    // Update heights
    y->height = max(mylib_height(y->left), mylib_height(y->right)) + 1;
    x->height = max(mylib_height(x->left), mylib_height(x->right)) + 1;

    // Return new root
    return x;
}


// Function to left rotate subtree rooted with x
Audiobook *leftRotate( Audiobook *x) 
{
     Audiobook*y = x->right;
     Audiobook*T2 = y->left;

    // Perform rotation
    y->left = x;
    x->right = T2;
    
    // Update heights
    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;

    // Return new root
    return y;
}

user *uleftRotate(user *x)
{
    user *y = x->right;
     user *T2 = y->left;

    // Perform rotation
    y->left = x;
    x->right = T2;
    
    // Update heights
    x->height = max(uheight(x->left), uheight(x->right)) + 1;
    y->height = max(uheight(y->left), uheight(y->right)) + 1;

    // Return new root
    return y;
}

mybook *mylib_leftRotate(mybook *x)
{
    mybook *y = x->right;
     mybook *T2 = y->left;

    // Perform rotation
    y->left = x;
    x->right = T2;
    
    // Update heights
    x->height = max(mylib_height(x->left), mylib_height(x->right)) + 1;
    y->height = max(mylib_height(y->left), mylib_height(y->right)) + 1;

    // Return new root
    return y;
}


// Get balance factor of node
int getBalance( Audiobook *node) 
{
    if (node == NULL)
        return 0;
    return height(node->left) - height(node->right);
}

int ugetBalance(user *node)
{
    if (node == NULL)
        return 0;
    return uheight(node->left) - uheight(node->right);
}

int mylib_getBalance(user *node)
{
    if (node == NULL)
        return 0;
    return mylib_height(node->left) - mylib_height(node->right);
}


// Function to insert a new node with given key in AVL tree
Audiobook *insert( Audiobook *node, HASHTYPE id,  char title[], char author[], char narrator[], char genre[], float min, float r, float t, float count, float sum, int **flag)
{
    // 1. Perform normal BST insertion
    if (node == NULL)
        return create_book_node(id, title, author, narrator, genre, min, r, t, count, sum);

    if (id < node->id)
        node->left = insert(node->left, id,  title, author, narrator, genre, min, r, t, count, sum, flag);
    else if (id > node->id)
        node->right = insert(node->right, id,  title, author, narrator, genre, min, r, t, count, sum, flag);
    else // Equal keys are not allowed in BST
    {
        *flag=0;
        return node;
    }

    // 2. Update height of this ancestor node
    if(*flag!=0)
    {
        node->height = 1 + max(height(node->left), height(node->right));

    // 3. Get the balance factor of this ancestor node to check whether
    //    this node became unbalanced
        int balance = getBalance(node);

    // If this node becomes unbalanced, then there are four cases

    // Left Left Case
        if (balance > 1 && id < node->left->id)
            return rightRotate(node); // root

    // Right Right Case
         if (balance < -1 && id > node->right->id)
            return leftRotate(node); // root

    // Left Right Case
        if (balance > 1 && id > node->left->id)
        {
            node->left = leftRotate(node->left);
            return rightRotate(node); // root
        }

    // Right Left Case
        if (balance < -1 && id < node->right->id) 
        {
            node->right = rightRotate(node->right);
            return leftRotate(node); // root
        }
    }
    // return the (unchanged) node pointer
    return node; // root
}

user *uinsert(user *node, HASHTYPE id, char name[], char mail[],int **flag)
{
    // 1. Perform normal BST insertion
    if (node == NULL)
        return create_user_node( id, name,  mail);

    if (id < node->uid)
        node->left = uinsert(node->left, id, name, mail, flag);
    else if (id > node->uid)
        node->right = uinsert(node->right, id, name, mail, flag);
    else // Equal keys are not allowed in BST
    {    
        *flag=0;
        
        return node;
    }

    if(*flag!=0)
    {
    // 2. Update height of this ancestor node
        node->height = 1 + max(uheight(node->left), uheight(node->right));

    // 3. Get the balance factor of this ancestor node to check whether
    //    this node became unbalanced
        int balance = ugetBalance(node);

    // If this node becomes unbalanced, then there are four cases

    // Left Left Case
        if (balance > 1 && id < node->left->uid)
             return urightRotate(node);

    // Right Right Case
        if (balance < -1 && id > node->right->uid)
            return uleftRotate(node);

    // Left Right Case
        if (balance > 1 && id > node->left->uid)
        {
            node->left = uleftRotate(node->left);
            return urightRotate(node);
        }

    // Right Left Case
        if (balance < -1 && id < node->right->uid) 
        {
            node->right = urightRotate(node->right);
            return uleftRotate(node);
        }
    }
    // return the (unchanged) node pointer
    return node;
}

mybook *mylib_insert(mybook *node, HASHTYPE id, float min, char title[], char author[], char narrator[], char genre[], float percent, float uRating, int ** flag)
{
    // 1. Perform normal BST insertion
    if (node == NULL)
        return create_mylib_node(id, min, title,  author, narrator,  genre, percent, uRating);

    if (id < node->id)
        node->left = mylib_insert(node->left, id, min, title, author, narrator, genre, percent, uRating, flag);
    else if (id > node->id)
        node->right = mylib_insert(node->right, id, min, title, author, narrator, genre, percent, uRating, flag);
    else // Equal keys are not allowed in BST
    {
        *flag=0;
        return node;
    }
        
    if(flag!=0)
    {

    // 2. Update height of this ancestor node
        node->height = 1 + max(mylib_height(node->left), mylib_height(node->right));

    // 3. Get the balance factor of this ancestor node to check whether
    //    this node became unbalanced
        int balance = mylib_getBalance(node);

    // If this node becomes unbalanced, then there are four cases

    // Left Left Case
        if (balance > 1 && id < node->left->id)
            return mylib_rightRotate(node); // root

    // Right Right Case
         if (balance < -1 && id > node->right->id)
            return mylib_leftRotate(node); // root

    // Left Right Case
        if (balance > 1 && id > node->left->id)
        {
            node->left = mylib_leftRotate(node->left);
            return mylib_rightRotate(node); // root
        }

    // Right Left Case
        if (balance < -1 && id < node->right->id) 
        {
             node->right = mylib_rightRotate(node->right);
            return mylib_leftRotate(node); // root
        }
    }
    // return the (unchanged) node pointer
    return node; // root
}


void Add_audiobook()
{
    char tit[SIZE], aut[SIZE], narr[SIZE], gen[SIZE];
    float min;
    printf("Enter the title of the book :\n");
    getchar();
    fgets(tit, SIZE, stdin);
    tit[strcspn(tit, "\n")] = '\0';

    printf("Enter the name of the author :\n");
    getchar();
    fgets(aut, SIZE, stdin);
    aut[strcspn(aut, "\n")] = '\0';

    printf("Enter the narrator's name :\n");
    getchar();
    fgets(narr, SIZE, stdin);
    narr[strcspn(narr, "\n")] = '\0';

    printf("Enter the genre:\n");
    getchar();
    fgets(gen, SIZE, stdin);
    gen[strcspn(gen, "\n")] = '\0';

    printf("Enter time duration of the book:\n");
    scanf("%f",min);
    
    Audiobook *lib=Library ;
    int flag=1;
    int *f_ptr=&flag;  
    lib=insert(lib, hash(strcat(tit,aut)), tit, aut, narr, gen, min , 0, 0, 0, 0, &f_ptr);
    if(*f_ptr==0)
        printf("Audiobook couldn't be added, its already there in the library!! ");
    else
        printf("Audiobook added successfully!!");
    Library=lib;
}

// edit audiobook()

// Function to find the node with minimum key value in a subtree rooted at node
Audiobook *minValueNode(Audiobook *node)
{
    Audiobook* current = node;
    while (current->left != NULL)
        current = current->left;
    return current;
}

// Recursive function to delete a node with given key from subtree with given root.
// It returns root of the modified subtree.
Audiobook *Delete_Audiobook(Audiobook *root, HASHTYPE key,bool* deleted)
{
    // delId=key

    // STEP 1: PERFORM STANDARD BST DELETE
    if (root == NULL)
        return root;

    // If the key to be deleted is smaller than the root's key, then it lies in left subtree
    if (key < root->id)
        root->left = Delete_Audiobook(root->left, key, deleted);

    // If the key to be deleted is greater than the root's key, then it lies in right subtree
    else if (key > root->id)
        root->right = Delete_Audiobook(root->right, key, deleted );

    // if key is same as root's key, then This is the node to be deleted
    else 
    {
        // Node with no children (leaf node)
        if (root->left == NULL && root->right == NULL) 
        {
            *deleted = true;
            free(root);
            return NULL;
        }
        // node with only one child or no child
        if ((root->left == NULL) || (root->right == NULL)) 
        {
            Audiobook *temp = root->left ? root->left : root->right;

            // No child case
            if (temp == NULL) 
            {
                temp = root;
                root = NULL;
            } 
            else // One child case
                *root = *temp; // Copy the contents of the non-empty child

            free(temp);
            *deleted = true;
        } 

        else
        {
            // node with two children: Get the inorder successor (smallest in the right subtree)
            Audiobook *temp = minValueNode(root->right);

            // Copy the inorder successor's data to this node
            root->id = temp->id;

            // Delete the inorder successor
            root->right = Delete_Audiobook(root->right, temp->id, deleted );
            *deleted = true;
        }
    }

    // If the tree had only one node then return
    if (root == NULL)
        return root;

    // STEP 2: UPDATE HEIGHT OF THE CURRENT NODE
    root->height = 1 + max(height(root->left), height(root->right));

    // STEP 3: GET THE BALANCE FACTOR OF THIS NODE (to check whether this node became unbalanced)
    int balance = getBalance(root);

    // If this node becomes unbalanced, then there are 4 cases

    // Left Left Case
    if (balance > 1 && getBalance(root->left) >= 0)
        return rightRotate(root);

    // Left Right Case
    if (balance > 1 && getBalance(root->left) < 0) 
    {
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }

    // Right Right Case
    if (balance < -1 && getBalance(root->right) <= 0)
        return leftRotate(root);

    // Right Left Case
    if (balance < -1 && getBalance(root->right) > 0) 
    {
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }

    return root;
}

void *Add_user_profile(char name[], char mail[])
{
    user *root=uList;
    HASHTYPE h=hash(strcat(name,mail));
    int flag=1;
    int *f_ptr=&flag;  
    root = uinsert(root, h, name, mail, &f_ptr);
    if(*f_ptr==0)
        printf("User already exists, duplicate entry not allowed !!");
    uList=root;
}


user *search_user(user *root, HASHTYPE key )
{
    // Base Cases: root is null or key is present at root
    if (root == NULL || hash(root->mail)==key)
        return root;

    // Key is greater than root's key
    if (hash(root->mail) < key)
        return search_user(root->right, key);

    // Key is smaller than root's key
    return search_user(root->left , key );
}

Audiobook *search_book(Audiobook *root, HASHTYPE key )
{
    // Base Cases: root is null or key is present at root
    if (root == NULL || hash(strcat(root->title, root->author))==key)
        return root;

    // Key is greater than root's key
    if (hash(strcat(root->title, root->author)) < key)
        return search_book(root->right, key);

    // Key is smaller than root's key
    return search_book(root->left , key );
}

void Add_to_User_library( user *result, Audiobook *book_found)
{
    mybook *my=result->mybooks;
    int flag=1;
    int *f_ptr=&flag;  
    my=mylib_insert(my, book_found->id, book_found-> min, book_found->title, book_found->author, book_found->narrator, book_found->genre, 0, 0, f_ptr);
    result->mybooks=my;
    if(*f_ptr!=0)
    {
        printf("Successfully updated the audiobook library!!\n");
        printf("The new library is as follows:\n");
        display_mylib(my);
    }

    else 
    {
        printf("Failed to add to user library!!\n");
        printf("Duplicate entry not allowed, the audiobook is already there in the audiobook!!\n");

    }
    
}

void display_mylib(mybook *root)
{
    if(root)
    {
        inorder(root->left);

        printf("Id.: %hu\t ", root->id);
        printf("Height: %d\n ", root->height);
        printf("Title: %s\t ", root->title);
        printf("Author: %s\t ", root->author);
        printf("Narrator: %s\t ", root->narrator);
        printf("Genre: %s\n ", root->genre);
        printf("Rating: %hd\n ", root->uRating);
        printf("Running time: %hd\n ", root->min);
        
        inorder(root->right);
    }
}

int strcasecmp( char *s1, char *s2) 
{
    while (*s1 && *s2 && (tolower(*s1) == tolower(*s2))) 
    {
        s1++;
        s2++;
    }
    return tolower(*s1) - tolower(*s2);
}

// Function to search for a value in AVL tree
void search_tit(Audiobook  *root,  char *title, int **flag) 
{
    if (root == NULL)
        return;

    int cmp = strcasecmp(title, root->title);

    // If the title matches, print the book's details
    if (cmp == 0)
    {
        *flag=0;
        printf("Title: %s\n", title);
        printf("Book id: %hu\n",root->id);
        getchar();
        printf("Author : %s\n", root->author);
        // Print other attributes of the book here
        printf("\n");
        // Continue searching for other books with the same title
        search_tit(root->left, title, flag );
        search_tit(root->right, title, flag );
    }
    // If the target title is less than the current node's title, search in the left subtree
    else if (cmp < 0)
        search_tit(root->left, title, flag );
    // If the target title is greater than the current node's title, search in the right subtree
    else
        search_tit(root->right, title, flag );
}

void search_aut(Audiobook  *root,  char *aut, int ** flag) 
{
    if (root == NULL)
        return;

    int cmp = strcasecmp(aut, root->author);

    // If the title matches, print the book's details
    if (cmp == 0)
    {
        *flag=0;
        printf("Author: %s\n", aut);
        printf("Book id: %hu\n",root->id);
        getchar();
        printf("Book title: %s\n", root->title);
        getchar();
        // Print other attributes of the book here
        printf("\n");
        // Continue searching for other books with the same author
        search_aut(root->left, aut, flag );
        search_aut(root->right, aut, flag );
    }
    // If the target author is less than the current node's author, search in the left subtree
    else if (cmp < 0)
        search_aut(root->left, aut, flag );
    // If the target author is greater than the current node's author, search in the right subtree
    else
        search_aut(root->right, aut, flag );
}


void search_genre(Audiobook  *root,  char *gen, int ** flag) 
{
    if (root == NULL)
        return;

    int cmp = strcasecmp(gen, root->genre);

    // If the title matches, print the book's details
    if (cmp == 0)
    {
        *flag=0;
        printf("Genre %s\n", gen);
        printf("Book id: %hu\n",root->id);
        getchar();
        printf("Book title: %s\n", root->title);
        getchar();
        // Print other attributes of the book here
        printf("\n");
        // Continue searching for other books with the same genre 
        search_genre(root->left, gen, flag );
        search_genre(root->right, gen, flag );
    }
    // If the target genre is less than the current node's genre, search in the left subtree
    else if (cmp < 0)
        search_genre(root->left, gen, flag);
    // If the target genre is greater than the current node's genre, search in the right subtree
    else
        search_genre(root->right, gen, flag);
}

Audiobook *search_tit_n_aut(Audiobook *root, HASHTYPE key)
{
    // Base Cases: root is null or key is present at root
    if (root == NULL || hash(strcat(root->title, root->author))==key)
        return root;

    // Key is greater than root's key
    if (hash(strcat(root->title, root->author)) < key)
        return search(root->right, key);

    // Key is smaller than root's key
    return search(root->left , key );
}

Audiobook *search_genre_n_aut(Audiobook *root, char tit[])
{
    // Base Cases: root is null or key is present at root
    if (root == NULL || strcmp(root->title,tit)==0)
        return root;

    // Key is greater than root's key
    if (strcmp(root->title,tit)<0)
        return search(root->right, tit);

    // Key is smaller than root's key
    return search(strcmp(root->title,tit)>0);
}

void Search_audiobook()
{
    Audiobook *lib = Library;
    int ch;
    char s1[SIZE], s2[SIZE];
    
    printf("Enter 1 to search by title:\n");
    printf("Enter 2 to search by author :\n");
    printf("Enter 3 to search by genre:\n");
    printf("Enter 4 to search by genre n author:\n");
    printf("Enter 5 to search by title n author:\n");
    printf("Enter ur choice:\n");

    scanf("%d", &ch);
    switch (ch)
    {
        case 1:

        lib=Library;
        printf("\nEnter the title of the book to be searched :");
        getchar();
        fgets(s1, SIZE, stdin);
        s1[strcspn(s1, "\n")] = '\0';
        int flag=1;
        int *f_ptr=&flag;
         search_tit(lib, s1, &f_ptr);
        if(*f_ptr==1)
            printf("No audiobooks found!!\n");
        // warna printing toh ho rha h fn() mei hee
        break;
        
        case 2 :

         lib=Library;
        printf("\nEnter the author of the book to be searched :");
        getchar();
        fgets(s1, SIZE, stdin);
        s1[strcspn(s1, "\n")] = '\0';
        int flag=1;
        int *f_ptr=&flag;
        search_aut(lib, s1,&f_ptr);
        if(*f_ptr==1)
            printf("No audiobooks found!!\n");
        // warna printing toh ho rha h fn() mei hee
        break;
 
        case 3:

         lib=Library;
        printf("\nEnter the genre u wanna search :");
        getchar();
        fgets(s1, SIZE, stdin);
        s1[strcspn(s1, "\n")] = '\0';
        int flag=1;
        int *f_ptr=&flag;
         search_genre(lib, s1, &f_ptr);
         if(*f_ptr==1)
            printf("No audiobooks found!!\n");
        // warna printing toh ho rha h fn() mei hee
        break;

        case 4:

         lib=Library;
        printf("\nEnter the title of the book to be searched:");
        getchar();
        fgets(s1, SIZE, stdin);
        s1[strcspn(s1, "\n")] = '\0';

        printf("\nEnter the name of the author to be searched:");
        getchar();
        fgets(s2, SIZE, stdin);
        s2[strcspn(s2, "\n")] = '\0';

        HASHTYPE key=hash(strcat(s1,s2));
        Audiobook *result = search_tit_n_aut(lib, key);
        if (result != NULL)
        {
            printf("Book(s) with such title %s exists in the Audiobook list\n", s1);
            printf("Book Title: %s\n", result->title);
            printf("Author :%s\n", result->author);
        }
            
        else
            printf("Book(s) with such title %s does not exist in the Audiobook list\n", s1);
        
        break ;

        case 5:

        printf("\nEnter the genre u wanna search:");
        getchar();
        fgets(s1, SIZE, stdin);
        s1[strcspn(s1, "\n")] = '\0';

        printf("\nEnter the name of the author to be searched:");
        getchar();
        fgets(s2, SIZE, stdin);
        s2[strcspn(s2, "\n")] = '\0';

        Audiobook *result = search_genre_n_aut(lib, key);
        if (result != NULL)
        {
            printf("Book(s) with such title %s exists in the Audiobook list\n", s1);
        }
            
        else
            printf("Book(s) with such title %s does not exist in the Audiobook list\n", s1);
        
        break; 

        default:
        printf("Invalid choice!!");

    }
}


void Listening_progress(mybook *result2, float m)
{
    update_Audiobook_totaltime(result2->id, (float)(result2->percent/100.0 *result2->min), m);
    /*NOW UPDATING USER TIME...*/
    result2->percent = 100.0 * ((float)m / result2->min);
}

void update_Audiobook_totaltime(HASHTYPE bookid, float old, float new)
{
    /*FIND THE BOOK*/
    Audiobook *lib = Library;
    lib=search_tit_n_aut(lib , bookid);
    
    if (lib != NULL)
        lib->total_listening_time += (new - old);
}

void mysearch_tit(mybook *root,  char *title, int **flag) 
{
    if (root == NULL)
        return;

    int cmp = strcasecmp(title, root->title);

    // If the title matches, print the book's details
    if (cmp == 0)
    {
        *flag=0;
        printf("Title: %s\n", title);
        printf("Book id: %hu\n",root->id);
        getchar();
        printf("Author : %s\n", root->author);
        // Print other attributes of the book here
        printf("\n");
        // Continue searching for other books with the same title
        mysearch_tit(root->left, title, flag);
        mysearch_tit(root->right, title, flag);
    }
    // If the target title is less than the current node's title, search in the left subtree
    else if (cmp < 0)
        mysearch_tit(root->left, title, flag);
    // If the target title is greater than the current node's title, search in the right subtree
    else
        mysearch_tit(root->right, title, flag);
}

void mysearch_aut(mybook  *root,  char *aut, int **flag) 
{
    if (root == NULL)
        return;

    int cmp = strcasecmp(aut, root->author);

    // If the title matches, print the book's details
    if (cmp == 0)
    {
        *flag=0;
        printf("Author: %s\n", aut);
        printf("Book id: %hu\n",root->id);
        getchar();
        printf("Book title: %s\n", root->title);
        getchar();
        // Print other attributes of the book here
        printf("\n");
        // Continue searching for other books with the same author
        mysearch_aut(root->left, aut, flag);
        mysearch_aut(root->right, aut, flag);
    }
    // If the target author is less than the current node's author, search in the left subtree
    else if (cmp < 0)
        mysearch_aut(root->left, aut, flag);
    // If the target author is greater than the current node's author, search in the right subtree
    else
        mysearch_aut(root->right, aut, flag);
}


void mysearch_genre(mybook *root,  char *gen, int **flag) 
{
    if (root == NULL)
        return;

    int cmp = strcasecmp(gen, root->genre);

    // If the title matches, print the book's details
    if (cmp == 0)
    {
        *flag=0;
        printf("Genre %s\n", gen);
        printf("Book id: %hu\n",root->id);
        getchar();
        printf("Book title: %s\n", root->title);
        getchar();
        // Print other attributes of the book here
        printf("\n");
        // Continue searching for other books with the same genre 
        mysearch_genre(root->left, gen, flag);
        mysearch_genre(root->right, gen, flag);
    }
    // If the target genre is less than the current node's genre, search in the left subtree
    else if (cmp < 0)
        mysearch_genre(root->left, gen, flag);
    // If the target genre is greater than the current node's genre, search in the right subtree
    else
       mysearch_genre(root->right, gen, flag);
}


mybook *mysearch_tit_n_aut(mybook *root, HASHTYPE key)
{
    // Base Cases: root is null or key is present at root
    if (root == NULL || hash(strcat(root->title, root->author))==key)
        return root;

    // Key is greater than root's key
    if (hash(strcat(root->title, root->author)) < key)
        return search(root->right, key);

    // Key is smaller than root's key
    return search(root->left , key );
}

void Display_user_libraries(user *u )
{
    // user found 
    // now apply search criteria 
    int ch;
    char s1[SIZE],s2[SIZE];
    printf("Enter 1 to search by title:\n");
    printf("Enter 2 to search by author :\n");
    printf("Enter 3 to search by genre:\n");
    printf("Enter 4 to search by genre n author:\n");
    printf("Enter 5 to search by title n author:\n");
    printf("Enter ur choice:\n");

    scanf("%d", &ch);
    switch (ch)
    {
        // u r displaying one by one, u gotta form a list of the filter criteria n display it in a sorted manner(as per listening time)
        case 1:

        printf("\nEnter the title of the book to be searched :");
        getchar();
        fgets(s1, SIZE, stdin);
        s1[strcspn(s1, "\n")] = '\0';
        int flag=1;
        int *f_ptr=&flag;
         mysearch_tit(u->mybooks, s1, &f_ptr);
        if(*f_ptr==1)
            printf("No audiobooks found!!\n");
        else
        {
            // display proper msg for no search results
        }

        break;
        
        case 2 :

        printf("\nEnter the author of the book to be searched :");
        getchar();
        fgets(s1, SIZE, stdin);
        s1[strcspn(s1, "\n")] = '\0';
        int flag=1;
        int *f_ptr=&flag;
        mysearch_aut(u->mybooks, s1 , &f_ptr);
        if(*f_ptr==1)
            printf("No audiobooks found!!\n");
        else
        {
            // display proper msg for no search results
        }

        break;
 
        case 3:

        printf("\nEnter the genre u wanna search :");
        getchar();
        fgets(s1, SIZE, stdin);
        s1[strcspn(s1, "\n")] = '\0';
        int flag=1;
        int *f_ptr=&flag;
         mysearch_genre(u->mybooks, s1, &f_ptr);
        
        if(*f_ptr==1)
            printf("No audiobooks found!!\n");
        else
        {
            // display proper msg for no search results
        }

        break;

        case 4:

        printf("\nEnter the title of the book to be searched:");
        getchar();
        fgets(s1, SIZE, stdin);
        s1[strcspn(s1, "\n")] = '\0';

        printf("\nEnter the name of the author to be searched:");
        getchar();
        fgets(s2, SIZE, stdin);
        s2[strcspn(s2, "\n")] = '\0';

        HASHTYPE key=hash(strcat(s1,s2));
        mybook *result = mysearch_tit_n_aut(u->mybooks, key);
        if (result != NULL)
        {
            printf("Book Details:\n");
            printf("Title: %s\n", result->title);
            printf("Author :%s\n", result->author);
        }
            
        else
            printf("No such book exists in the user library!!");
        
        break ;

        case 5:
        
        // filter criteria : genre +author 
        // yet not completed 

        default:
        printf("Invalid choice!!");

    }
}


void Rate_Audiobook(mybook *result2, float user_rat)
{
    update_audiobook_rating(result2->id, user_rat);
    result2->uRating = user_rat ;
    printf("\n\tYOUR RATING OF BOOK %s IS NOW %hu*", result2->title, user_rat);
}

void update_audiobook_rating(HASHTYPE bookid, float r )
{
    // FIND THE BOOK 
    Audiobook *lib = Library;
    lib=search_tit_n_aut(lib , bookid);
    if (lib != NULL)
    {
        lib->rating = ((lib->rating) * (lib->count_rating) + r) / (lib->count_rating + 1);
        lib->count_rating++;
    }
}


void Most_popular_audiobook(Audiobook *root, float *max, Audiobook **ans)
{

    if (root != NULL)
    {
        float k = (root->sum_rating / root->count_rating);
        if (k > (*max))
        {
            (*max) = k;
            (*ans) = root;
        }

        Most_popular_audiobook(root->left, max, ans);
        // float k=(root->sum_rating/root->count_rating);
        //  printf("max is %f\n",max);
        //     printf("kis %f\n",k);

        Most_popular_audiobook(root->right, max, ans);
    }
    return;
}


void Listening_history_report_user(HASHTYPE uid)
{
    user *userList=uList;
    user *found=search_user(userList, uid );
    if(found==NULL)
        printf("user id invalid, no such user exists");

    else 
    {
        // user found
        float *total;
        mybook *my=found->mybooks;
        if(my==NULL)
            printf("The above user does not have his/her personal audiobook library");
        else 
        {
            details(my,&total);
            printf("Total listening time is: %f", *total);
        }
    }
}

void details(mybook *my, float **total)
{
    while(my!=NULL)
    {
        details(my=my->left, total) ;
        *total += (my->percent) * (my->min) / 100.0;
        printf("\n%30s%30s%30f%20hu", my->title, my->author, my->percent,my->uRating);
        details(my=my->right, total) ;
    }
}

void Listening_history_report_audiobook(HASHTYPE bookid)
{
    Audiobook *lib=Library;
    Audiobook *result= search_tit_n_aut(lib, bookid);
    if(result==NULL)
        printf("No such book exists in the library!! ");
    else 
    {
        printf("Title: %s \t Author: %s\n",result->title,result->author);
        printf("Total listening time of the audiobook %f",result-> total_listening_time );
    }
}


// Function to print inorder traversal of the AVL tree
void inorder(Audiobook *root) 
{
    if (root) 
    {
        inorder(root->left);

        printf("Id.: %hu\t ", root->id);
        printf("Height: %d\n ", root->height);
        printf("Title: %s\t ", root->title);
        printf("Author: %s\t ", root->author);
        printf("Narrator: %s\t ", root->narrator);
        printf("Genre: %s\n ", root->genre);
        printf("Rating: %hd\n ", root->rating);
        printf("Running time: %hd ", root->min);
        
        inorder(root->right);
    }
}

void Range_Search_Audiobook(Audiobook* root, HASHTYPE id1, HASHTYPE id2, int* found)
{
    if (root != NULL)
    {
        Range_Search_Audiobook(root->left,id1,id2,found);
        if( root->id < id2 && root->id > id1 )
        {
           printf("ID: %d\t Title  %s\n", root->id, root->title);
           (*found)=1;

        }
        Range_Search_Audiobook(root->right,id1,id2,found);
    }
    return ;
}


void main() 
{
    FILEREADER();
    Audiobook *books=Library;
    user *userlist=uList;
    printf("Inorder traversal of the constructed AVL tree is \n\n");
    inorder(books);
    books=Library;
   // struct Node *root = NULL;
    /* Constructing tree given in the above figure */
   /* root = insert(root, 9);
    root = insert(root, 5);
    root = insert(root, 10);
    root = insert(root, 0);
    root = insert(root, 6);
    root = insert(root, 11);
    root = insert(root, -1);
    root = insert(root, 1);
    root = insert(root, 2);

    

    root = deleteNode(root, 10);

    
printf("\nInorder traversal after deletion of 10 \n");
    inorder(root);
    */
    printf("Enter ur choice:");
    int ch;
    switch(ch)
    {
        case 1:
        
        Add_audiobook();
        Audiobook *lib=Library;
        printf("The audiobook list after adding the new book would be:");
        inorder(lib);

        break;

        case 2:

        char tit[SIZE],aut[SIZE];
        printf("Enter the title of the audiobook n the name of its author :");
        getchar();
        fgets(tit, SIZE, stdin);
        tit[strcspn(tit, "\n")] = '\0';

        getchar();
        fgets(aut, SIZE, stdin);
        aut[strcspn(aut, "\n")] = '\0';

        HASHTYPE  h=HASH(strcat(tit,aut));
        bool deleted=false;
        books=Library;
        Library=Delete_Audiobook(books, h, &deleted);
        books=Library;
        if (!deleted) 
        {
            printf("\nSuch audiobook doesnt exist in the audiobook list\n");
        }   
        else 
        {
            printf("Deletion successful!!");
            printf("Inorder traversal of the audiobook tree after deleting the given audiobook is \n");
            inorder(books);
        }

        break;

        case 3:

        char name[SIZE],mail[SIZE];
        printf("Enter the name and email id of the user u wanna add:");
        getchar();
        fgets(name, SIZE, stdin);
        name[strcspn(name, "\n")] = '\0';

        getchar();
        fgets(mail, SIZE, stdin);
        mail[strcspn(mail, "\n")] = '\0';

        Add_user_profile(name,mail);
        printf("After changes the user list looks as follows:\n");
        printf("Inorder traversal:\n");
        userlist=uList;
        inorder(userlist);

        break ;

        case 4:

        char mail[SIZE];
        char tit[SIZE],aut[SIZE];

        printf("Enter the title of the book:");
        getchar();
        fgets(tit, SIZE, stdin);
        tit[strcspn(tit, "\n")] = '\0';

        printf("Enter the author of the book:");
        getchar();
        fgets(aut, SIZE, stdin);
        aut[strcspn(aut, "\n")] = '\0';

        Audiobook *book_found=Library;
        book_found=search_book(book_found,hash(strcat(tit,aut)));

        if(book_found==NULL)
            printf("No such book exists in the library, cant be added to any users account!!\n");

        else 
        {
            // book exists 

            printf("Enter the user id whose audiobook library u wish to update:");
            getchar();
            fgets(mail, SIZE, stdin);
            mail[strcspn(mail, "\n")] = '\0';

            user *result =uList;
            result= search_user(result, hash(mail));

            if(result==NULL)
            {
                printf("user id invalid, no such user exists!!\n ");
                printf("Cant add audiobook to such account\n");
            }
                
            else
            {
                // user exists
                Add_to_User_library(result, book_found);
            }
        }
        
        break;

        case 5:

        Search_audiobook();

        case 6 :

        books=Library;
        userlist=uList;

        char mail[SIZE], tit[SIZE], aut[SIZE];
        float extra;
        printf("Enter ur mail id: ");
        getchar();
        fgets(mail, SIZE, stdin);
        mail[strcspn(mail, "\n")] = '\0';
        user *result=search_user(uList, hash(mail));
        
        if(result==NULL)
            printf("No such user exists");

        else 
        {
            // user exists
            // root of the user library
            mybook *my = result->mybooks;
            
            printf("Enter title of the book whose progress is to be updated: ");
            getchar();
            fgets(tit, SIZE, stdin);
            tit[strcspn(tit, "\n")] = '\0';

            printf("Enter name of the author of the above book: ");
            getchar();
            fgets(aut, SIZE, stdin);
            aut[strcspn(aut, "\n")] = '\0';
            
            mybook *result2= search_book( my, hash(strcat(tit,aut)));

            if(result2==NULL)
            {
                printf("No such audiobook exists in the users' personal library ");
            }

            else 
            {
                // book found
                printf("Enter the extra mins u have watched ");
                scanf("%f",extra);
                // result : node corresponding to the user 
                // result2 : node corresponding to a particular book in the users' library
                Listening_progress(result2, extra);
            }
        }

        break;

        case 7 : 

        userlist=uList;
        char mail[SIZE];
        printf("Enter ur mail id: ");
        getchar();
        fgets(mail, SIZE, stdin);
        mail[strcspn(mail, "\n")] = '\0';
        user *result=search_user(uList, hash(mail));
        if(result==NULL)
            printf("User id incorrect, no such user exists!!\n");
        else
            Display_user_libraries(result);

        break;
        // yet to be completed

        case 8 :

        books=Library;
        userlist=uList;

        char mail[SIZE], tit[SIZE], aut[SIZE];
        float user_rat;
        printf("Enter ur mail id: ");
        getchar();
        fgets(mail, SIZE, stdin);
        mail[strcspn(mail, "\n")] = '\0';
        user *result=search_user(uList, hash(mail));
        
        if(result==NULL)
            printf("user id incorrect, no such user exists");

        else 
        {
            // user exists
            // root of the user library
            mybook *my = result->mybooks;
            
            printf("Enter title of the book whose progress is to be updated: ");
            getchar();
            fgets(tit, SIZE, stdin);
            tit[strcspn(tit, "\n")] = '\0';

            printf("Enter name of the author of the above book: ");
            getchar();
            fgets(aut, SIZE, stdin);
            aut[strcspn(aut, "\n")] = '\0';
            
            mybook *result2= search_book( my, hash(strcat(tit,aut)));

            if(result2==NULL)
            {
                printf("No such audiobook exists in the users' personal library ");
            }

            else 
            {
                // book found
                printf("Rate the book !!");
                scanf("%f",user_rat);
                // result : node corresponding to the user 
                // result2 : node corresponding to a particular book in the users' library
                Rate_Audiobook(result2, user_rat);
            }
        }
        
        break;

        case 9 :
        Audiobook *lib=Library;
        Audiobook *ans = NULL;
        float max = 0.0;
        Most_popular_audiobook(lib, &max, &ans);

        if (ans == NULL)
        {
            printf("Audio books are not rated yet\n");
        }

        else
        {
            printf(" The Most Popular Audiobook Title : %s Author : %s Average rating : %f", ans->title, ans->author, (ans->rating));
        }
        
        break;

        case 10 : 
        
        char mail[SIZE];
        printf("Enter the mail id of the user whose listening history is to be displayed:");
        getchar();
        fgets(mail, SIZE, stdin);
        mail[strcspn(mail, "\n")] = '\0';

        Listening_history_report_user(hash(mail));

        break;

        case 12:
        
        HASHTYPE bookid;
        printf("Enter the bookid");
        scanf("%hu", &bookid);
        Listening_history_report_audiobook(bookid);

        break;

        case 13 :

        Audiobook *root=Library;
        HASHTYPE id1, id2;
        printf("Enter the id of audiobook 1:");
        scanf("%hu",&id1);

        printf("Enter the id of audiobook 2:");
        scanf("%hu",&id2);
        
        if(id1>id2)
        {  
            int temp=id1;
            id1=id2;
            id2=temp;
        }
        
        if(id1==id2)
        {
            printf("Both are same\n ");
        }
    
        else
        {
            int found=0;
            Range_Search_Audiobook (root,id1, id2, &found);

            if(found==0)
            {
                printf("NO Audio books exist between the audio book id %d and audio book id %d\n",id1,id2);
            }

            else
            {
                printf("the above are the  Audio books  between the audio book id %d and audio book id %d\n",id1,id2);
            }
        }

        break;

        default :

        printf("Invalid choice!!");
    }
}
