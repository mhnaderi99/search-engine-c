#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>
#define MAX 100
#define WHITE 7
#define GREEN 10
#define RED 12

struct node{
    char *name;
    char *filename;
    int repeat;
    struct node *next;
};

struct result{
    char *name;
    int repeat;
    struct result *next;
};

struct bstNode{
    char *name;
    struct bstNode *left;
    struct bstNode *right;
    struct result *files;
    struct bstNode *next;
    struct bstNode *prev;
};

struct cor{
    char *word;
    int dis;
};

struct node *root, *cur;
struct bstNode *bstroot, *Qroot, *Qcur;
char pathes[MAX][MAX],filesNames[MAX][MAX];
int numFiles=0;
char skipWords[MAX][MAX];
int numSkipWords=0;
int skipFlag=0;
struct cor top3[3];

struct node* cnode(char *word, char *filename){
    cur->next=(struct node*)malloc(sizeof(struct node));
    cur->next->name=word;
    cur->next->filename=filename;
    cur->next->next=NULL;
    return cur->next;
}

struct bstNode* Qnode(struct bstNode *nod){
    Qcur->next=(struct bstNode*)malloc(sizeof(struct bstNode));
    Qcur->next->files=(struct result*)malloc(sizeof(struct result));
    Qcur->next->name=nod->name;
    Qcur->next->files=nod->files;
    Qcur->next->next=NULL;
    return Qcur->next;

};

void clearCor(){
    int i;
    for(i=0;i<3;i++){
        top3[i].word="";
        top3[i].dis=MAX;
    }
}

void sortCor(){
    int i,j;
    for(i=0;i<3;i++){
        for(j=0;j<2;j++){
            if(top3[j].dis>top3[j+1].dis){
                struct cor tmp=top3[j];
                top3[j]=top3[j+1];
                top3[j+1]=tmp;
            }
        }
    }
}

void SetColor(int ForgC){
    WORD wColor;
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if(GetConsoleScreenBufferInfo(hStdOut, &csbi)){
        wColor = (csbi.wAttributes & 0xF0) + (ForgC & 0x0F);
        SetConsoleTextAttribute(hStdOut, wColor);
    }
    return;
}

char toLower(char c){
    if(c>='A' && c<='Z'){c+=('a'-'A');}
    return c;
}

char *toLowerString(char *s){
    char *r=(char*)malloc(MAX*sizeof(char));
    int l=strlen(s),i;
    for(i=0;i<=l;i++){
        r[i]=toLower(s[i]);
    }
    return r;
}

int mini(int a, int b){
    if(a<b){return a;}
    return b;
}

int LD(char *a, char *b){
    int d[MAX][MAX];
    int n=strlen(a);
    int m=strlen(b);
    int i,j;
    for(j=0;j<=m;j++){
        for(i=0;i<=n;i++){
            d[j][i]=0;
        }
    }
    if(n==0){return m;}
    if(m==0){return n;}

    for(i=0;i<=n;i++){
        d[0][i]=i;
    }
    for(j=0;j<=m;j++){
        d[j][0]=j;
    }
    for(j=1;j<=m;j++){
        for(i=1;i<=n;i++){
            int cost=(a[i-1]!=b[j-1]);
            int p=mini(d[j][i-1]+1,d[j-1][i]+1);
            int c=mini(cost+d[j-1][i-1],p);
            d[j][i]=c;
        }
    }


    return d[m][n];
}

char *fileName(char *path){
    char *name=(char*)malloc(MAX*sizeof(char));
    int l=strlen(path);
    int i,cu=0;
    for(i=0;i<l;i++){
        if(path[i]==92){cu=i;}
    }
    cu++;
    for(i=cu;i<l;i++){
        name[i-cu]=path[i];
    }
    name[l-cu]=0;
    return path;
    //return name;
}

char *NameOfFile(char *path){
    char *name=(char*)malloc(MAX*sizeof(char));
    int l=strlen(path);
    int i,cu=0;
    for(i=0;i<l;i++){
        if(path[i]==92){cu=i;}
    }
    cu++;
    for(i=cu;i<l;i++){
        name[i-cu]=path[i];
    }
    name[l-cu]=0;
    return name;
}

int checkFormat(char *path, char *format){
    int l=strlen(path);
    int l2=strlen(format);
    if(l2>=l){return 0;}
    int i;
    for(i=l-l2;i<l;i++){
        if(toLower(path[i])!=toLower(format[i-(l-l2)])){return 0;}
    }
    return 1;
}

int isSkippedWord(char *word){
    int i;
    for(i=0;i<numSkipWords;i++){
        if(strcmp(toLowerString(word),toLowerString(skipWords[i]))==0){
            return 1;
        }
    }
    return 0;
}



void readSkippedWords(char *filePath){
    FILE *f=fopen(filePath,"r");
    char *word=(char*)malloc(MAX*sizeof(char));
    int t=0;
    while(!feof(f)){
        fscanf(f,"%s",word);
        strcpy(skipWords[t],word);
        t++;
    }
    numSkipWords=t;
    fclose(f);
}

int walkDir(char *basedir, int skip){
	DIR *dir;
	char b[MAX];
	struct dirent *ent;
	dir = opendir(basedir);
	if(dir != NULL){
		while((ent = readdir(dir)) != NULL){
			// do not allow "." or ".."
			if(strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0){
				continue;
			}

			char entpath[] = "";
			strcat(entpath, basedir);
			strcat(entpath, "\\");
			strcat(entpath, ent->d_name);
			if(isDir(entpath)){ // directory
				walkDir(entpath,skip);
			}
			else{ // file
                char filePath[MAX]={""};
                strcat(filePath,basedir);
                strcat(filePath,"\\");
                strcat(filePath,ent->d_name);
                if(checkFormat(filePath,".txt")){
                    if(strcmp(toLowerString(ent->d_name),"skipword.txt")!=0){
                        if(skip==0){
                            strcpy(pathes[numFiles],filePath);
                            numFiles++;
                        }
                    }
                    else{
                        if(skip==1){
                            readSkippedWords(filePath);
                        }
                    }
                }
			}
		}

		closedir(dir);
		return 1;
	}
	else{
		fprintf(stderr, "\nFailed to walk directory \"%s\"\n", basedir);
		if(1){
			perror("opendir()");
		}
		return 0;
	}
}


int isDir(const char *file_path)
{
	struct stat s;
	stat(file_path, &s);
	return S_ISDIR(s.st_mode);
}


void readFile(char *path){
    char *filename=(char*)malloc(MAX * sizeof(char));
    filename=fileName(path);
    FILE *fp;
    fp=fopen(path,"r");
    while(!feof(fp)){
        char *word=(char*)malloc(MAX * sizeof(char));
        fscanf(fp,"%s", word);
        if(!isSkippedWord(word) && !hasSign(word,0)){
            cur = cnode(word,filename);
        }
    }
    fclose(fp);
}


struct bstNode* check(struct bstNode *r, char *word){
    if(strcmp(toLowerString(word),toLowerString(r->name))==0){return r;}
    if(strcmp(toLowerString(word),toLowerString(r->name))==1){
        if(r->right!=NULL){
            return check(r->right,word);
        }
        else{
            struct bstNode *blank=NULL;
            blank=(struct bstNode*)malloc(sizeof(struct bstNode));
            blank->files=(struct result*)malloc(sizeof(struct result));
            blank->files->next=(struct result*)malloc(sizeof(struct result));
            blank->files->next->name="";
            blank->name="";
            return blank;
        }
    }
    if(strcmp(toLowerString(word),toLowerString(r->name))==-1){
        if(r->left!=NULL){
            return check(r->left,word);
        }
        else{
            struct bstNode *blank=NULL;
            blank=(struct bstNode*)malloc(sizeof(struct bstNode));
            blank->files=(struct result*)malloc(sizeof(struct result));
            blank->files->next=(struct result*)malloc(sizeof(struct result));
            blank->files->next->name="";
            blank->name="";
            return blank;
        }
    }
}


void insertToBST(struct bstNode *r, struct node *nod){
    if(strcmp(toLowerString(nod->name),toLowerString(r->name))==0){
        struct result *tmp=r->files->next;
        while(strcmp(toLowerString(tmp->name),toLowerString(nod->filename))!=0 && tmp->next!=NULL){
            tmp=tmp->next;
        }
        if(strcmp(toLowerString(tmp->name),toLowerString(nod->filename))==0){tmp->repeat=tmp->repeat+1;}
        else if(tmp->next==NULL){
            tmp->next=(struct result*)malloc(sizeof(struct result));
            tmp->next->name=nod->filename;
            tmp->next->repeat=1;
        }
        else {add_filename(r,nod->filename);}
    }
    if(strcmp(toLowerString(nod->name),toLowerString(r->name))==1){
        if(r->right==NULL){
            r->right=(struct bstNode*)malloc(sizeof(struct bstNode));
            r->right->files=(struct result*)malloc(sizeof(struct result));
            r->right->files->next=(struct result*)malloc(sizeof(struct result));
            r->right->files->name="";
            r->right->files->repeat=0;
            r->right->files->next->repeat=1;
            r->right->name=nod->name;
            r->right->files->next->name=nod->filename;
            r->right->files->next->next=NULL;
            r->right->right=NULL;
            r->right->left=NULL;
        }
        else {
            insertToBST(r->right, nod);
        }
    }
    if(strcmp(toLowerString(nod->name),toLowerString(r->name))==-1){
        if(r->left==NULL){
            r->left=(struct bstNode*)malloc(sizeof(struct bstNode));
            r->left->files=(struct result*)malloc(sizeof(struct result));
            r->left->files->next=(struct result*)malloc(sizeof(struct result));
            r->left->files->name="";
            r->left->files->repeat=0;
            r->left->files->next->repeat=1;
            r->left->name=nod->name;
            r->left->files->next->name=nod->filename;
            r->left->files->next->next=NULL;
            r->left->right=NULL;
            r->left->left=NULL;
        }
        else {
            insertToBST(r->left, nod);
        }
    }
}

void add_filename(struct bstNode *b, char *filename){
    char *fn=(char*)malloc(MAX*sizeof(char));
    if(b->files->next==NULL){
        b->files->next=(struct result*)malloc(sizeof(struct result));
        b->files->next->name=filename;
        b->files->next->next=NULL;
        return;
    }
    struct result* cur=b->files->next;
    int flag=1;
    while(flag){
        fn=cur->name;
        if(strcmp(fn,filename)==0){return;}
        if(cur->next!=NULL){cur=cur->next;}
        else {flag=0;}
    }
    cur->next=(struct result*)malloc(sizeof(struct result));
    cur->next->name=filename;
    cur->next->next=NULL;
}

void LLtoBST(){

    struct node *temp;
    temp=root->next;
    while(temp!=NULL){
        char *word=(char*)malloc(MAX * sizeof(char));
        word=temp->name;
        insertToBST(bstroot,temp);
        temp=temp->next;
    }
}

void print_LL()
{
    struct node *temp;
    temp=root->next;
    while(temp!=NULL){
        printf("%s %s\n", temp->name, temp->filename);
        temp=temp->next;
    }
}

void printBST(struct bstNode *r)
{
    if(r->left!=NULL){printBST(r->left);}
    if(r->right!=NULL){printBST(r->right);}
    printf("%s: ", r->name);
    struct result *temp=r->files->next;
    while(temp!=NULL){
        printf("<%s *%d> ", NameOfFile(temp->name), temp->repeat);
        temp=temp->next;
    }
    printf("\n");
    return;
}

int isEmpty(int top){
    if(top==MAX-1){return 1;}
    return 0;
}

int isFull(int top){
    if(top==-1){return 1;}
    return 0;
}

void push(char *arr, int *top, char inp){
    if(!isFull(*top)){
        arr[*top]=inp;
        (*top)--;
        return;
    }
    printf("Stack is Full\n");
}

char pop(char *arr, int *top){
    if(!isEmpty(*top)){
        (*top)++;
        return arr[*top];
    }
    printf("Stack is Empty\n");
}

void push_s(char arr[MAX][MAX], int *top, char *inp){
    if(!isFull(top)){
        strcpy(arr[*top],inp);
        (*top)--;
        return;
    }
}

char* pop_s(char arr[MAX][MAX], int *top){
    if(!isEmpty(top)){
        (*top)++;
        return arr[*top];
    }
}

void print(char *arr, int top){
    int i;
    for(i=MAX-1;i>=top+1;i--){
        printf("%c ", arr[i]);
    }
    printf("\n");
}

int isNum(char c){
    if(c=='&' || c=='^' || c=='|' || c=='!' || c=='(' || c==')' || c==' '){return 0;}
    return 1;
}

int toNum(char c){
    if(isNum(c)){return c-'0';}
    return 0;
}

int isOperator(char c){
    if(c=='&' || c=='|' || c=='^' || c=='!'){return 1;}
    return 0;
}

int operatorVal(char c){
    int val=0;
    if(c=='!'){val=1;}
    return val;
}

int isHigherThan(char a, char b){
    if(!isOperator(a) || !isOperator(b)){return 0;}
    return (operatorVal(a)>=operatorVal(b));
}

int and(int a, int b){return a&&b;}
int or(int a, int b){return a||b;}
int not(int a){return !a;}
int xor(int a, int b){return a!=b;}

char * infixToPostfix(char *infix){
    char elements[MAX][MAX];
    int cur=0;
    int l=strlen(infix);
    infix[l]=' ';infix[l+1]=')';infix[l+2]=0;
    char stack[MAX]={""};int top=MAX-1;
    push(stack,&top,'(');
    char *postfix=(char*)malloc(MAX*sizeof(char));
    int i;
    for(i=0;i<l+2;i++){
        if(isNum(infix[i])){
            while(isNum(infix[i])){
                postfix[cur]=infix[i];
                cur++;
                i++;
            }
            postfix[cur]=' ';cur++;

        }
        if(infix[i]=='('){
            push(stack,&top,'(');
        }
        if(isOperator(infix[i])){
            while(isHigherThan(stack[top+1],infix[i]) && !(infix[i]=='!' && stack[top+1]=='!')){
                postfix[cur]=pop(stack,&top);
                cur++;
                postfix[cur]=' ';
                cur++;
            }
            push(stack,&top,infix[i]);
        }
        if(infix[i]==')'){
            while(stack[top+1]!='('){
                postfix[cur]=pop(stack,&top);
                cur++;
                postfix[cur]=' ';
                cur++;
            }
            pop(stack,&top);
        }
    }
    infix[l]=0;
    postfix[cur]=0;
    return postfix;

}

int whichFile(char *filename){
    int i;
    for(i=0;i<numFiles;i++){
        if(strcmp(filename,filesNames[i])==0){return i;}
    }
    return -1;
}

void Operate(char op){

    if(op=='!'){
        struct bstNode *alt=(struct bstNode*)malloc(sizeof(struct bstNode));
        alt->files=(struct result*)malloc(sizeof(struct result));
        alt->files->next=(struct result*)malloc(sizeof(struct result));
        alt->files->next=NULL;
        struct bstNode *tmp=Qroot;
        while(tmp->next!=NULL){
            tmp=tmp->next;
        }
        struct result* f = tmp->files->next;
        int t=0;
        while(f!=NULL){
            t++;
            f=f->next;
        }
        f=tmp->files->next;
        int i;
        int ch[MAX]={0};
        for(i=0;i<t;i++){
            ch[whichFile(f->name)]++;
            f=f->next;
        }
        for(i=0;i<numFiles;i++){
            if(ch[i]==0){
                add_filename(alt,filesNames[i]);
            }
        }
        tmp->files=alt->files;

        return;
    }

    struct bstNode *alt=(struct bstNode*)malloc(sizeof(struct bstNode));
    alt->files=(struct result*)malloc(sizeof(struct result));
    alt->files->next=(struct result*)malloc(sizeof(struct result));
    alt->files->next=NULL;
    struct bstNode *tmp1=Qroot, *tmp2;

    while(tmp1->next!=NULL && tmp1->next->next!=NULL){
        tmp1=tmp1->next;
        tmp2=tmp1->next;
    }
    struct result* f = tmp1->files->next;
    int t=0;
    while(f!=NULL){
        t++;
        f=f->next;
    }

    f = tmp1->files->next;
    int ch[MAX]={0},i;
    for(i=0;i<t;i++){
        ch[whichFile(f->name)]++;
        f=f->next;
    }
    t=0;
    f = tmp2->files->next;
    while(f!=NULL){
        t++;
        f=f->next;
    }
    f=tmp2->files->next;
    for(i=0;i<t;i++){
        ch[whichFile(f->name)]++;
        f=f->next;
    }

    if(op=='&'){
        for(i=0;i<numFiles;i++){
            if(ch[i]==2){
                add_filename(alt,filesNames[i]);
            }
        }
    }
    if(op=='^'){
        for(i=0;i<numFiles;i++){
            if(ch[i]==1){
                add_filename(alt,filesNames[i]);
            }
        }
    }
    if(op=='|'){
        for(i=0;i<numFiles;i++){
            if(ch[i]>0){
                add_filename(alt,filesNames[i]);
            }
        }
    }
    tmp1->next=NULL;
    tmp1->files=alt->files;
    Qcur=tmp1;
}

int howMany(char *word, char *filename){
    struct bstNode *b=check(bstroot,word);
    if(strcmp(b->name,"")==0){return 0;}
    struct result *f=b->files->next;
    while(f->next!=NULL && strcmp(filename,f->name)!=0){
        f=f->next;
    }
    if(strcmp(filename,f->name)==0){return f->repeat;}
    return 0;
}

int isSign(char c, int flag){
    if(!flag){if(c=='.' || c=='!' || c=='?' || c==',' || c=='"' || c==39){return 1;}}
    if(flag){if(c=='.' || c=='!' || c=='?'){return 1;}}
    return 0;
}

int hasSign(char *s, int flag){
    int l=strlen(s),i;
    for(i=0;i<l;i++){
        char c=s[i];
        if(isSign(c,flag)){return 1;}
    }
    return 0;
}

void printSentences(char *filename, char *word){
    int num=0;
    char *text=(char*)malloc(MAX*sizeof(char));
    char *token=(char*)malloc(MAX*sizeof(char));
    int from=0, to=0,l=2,i,j,l2=strlen(word),cursor=0;
    text[0]='.';text[1]=' ';
    FILE *f=fopen(filename,"r");
    while(!feof(f)){
        char c=fgetc(f);
        if(c!='\n'){text[l]=c;l++;}
    }
    l--;
    text[l]=' ';
    text[l+1]='.';
    text[l+2]=0;
    l=l+2;
    for(i=0;i<l-l2+1;i++){
        if(isSign(text[i],1)){from=i+2;continue;}
        if(text[i]==' '){continue;}
        char tcmp[MAX]={""};
        for(j=i;j<l2+i;j++){
            tcmp[j-i]=text[j];
        }
        tcmp[l2]=0;
        if(strcmp(toLowerString(word),toLowerString(tcmp))==0 && text[i-1]==' ' && text[i+l2]==' '){
            int start=i;
            i+=l2;
            while(!isSign(text[i],1)){i++;}
            to=i;
            char *sentence=(char*)malloc(MAX*sizeof(char));
            strncpy(sentence,text+from,to-from+1);
            num++;
            printf("\t\t%d. ",num);
            int k,L=strlen(sentence);
            sentence[L]=0;
            int flag=1;
            for(k=0;k<L && flag;k++){
                if(k+from<start){SetColor(WHITE);}
                if(k+from>=start && k+from<start+l2){SetColor(GREEN);}
                if(k+from>=start+l2){SetColor(WHITE);}
                printf("%c",sentence[k]);
                if(isSign(sentence[k],1)){flag=0;}
            }

            //printf("%s",sentence);
            strcpy(sentence,"");
            printf("\n");
            from=to+2;
        }
    }
    fclose(f);
    //printf("\n");

}

int printFiles(struct bstNode *b, char words[MAX][MAX], int n, int isSingle){
    struct result *f=b->files->next;
    if(f==NULL){return -1;}
    if(isSingle==0){
        while(f!=NULL){
            printf("\t%s:\t",NameOfFile(f->name));
            int i;
            for(i=0;i<n;i++){
                printf("<%s*%d> ", words[i],howMany(words[i],f->name));
            }
            printf("\n");
            f=f->next;
        }
        printf("\n");
        return 1;
    }
}

// This is & That &
// This & is & That

int isNew(char words[MAX][MAX], char *m, int n){
    int i;
    for(i=0;i<n;i++){
        if(strcmp(toLowerString(words[i]),toLowerString(m))==0){return 0;}
    }
    return 1;
}

int processQuery(char *query, int single){
    if(single==1){
        struct bstNode *tmp=check(bstroot,query);
        if(strlen(tmp->name)==0){return 3;}
        printf("\n");
        struct result *f=tmp->files->next;
        while(f!=NULL){
            printf("\t%s:\n", NameOfFile(f->name));
            printSentences(f->name,query);
            f=f->next;
        }
        return -1;
    }
    int numWords=0;
    char *m=(char*)malloc(MAX*sizeof(char));
    char words[MAX][MAX];
    int l=strlen(query),i,s=0,t=0,o=0;
    for(i=0;i<l;i++){
        if(query[i]==' '){
            strncpy(m,query+s,i-s);
            m[i-s]=0;
            if(isOperator(m[0]) && strlen(m)==1){
                o++;
                Operate(m[0]);
            }
            else{
                t++;
                if(isNew(words,m,numWords)){
                    strcpy(words[numWords],m);
                    numWords++;
                }
                struct bstNode *tmp=check(bstroot,m);
                Qcur=Qnode(tmp);
            }
            s=i+1;
        }
    }
    if(Qroot->next->next!=NULL){return 0;}
    else{
        int err=(printFiles(Qroot->next,words,numWords, single));
        if(err==-1){return 8;}
        if(err==1){return 7;}
    }
}

int hasSkippedWord(char *query){
    char *qtmp=(char*)malloc(MAX*sizeof(char));
    sprintf(qtmp," %s ",query);
    int i;
    for(i=0;i<numSkipWords;i++){
        char *tmp=(char*)malloc(MAX*sizeof(char));
        sprintf(tmp," %s ",skipWords[i]);
        if(strstr(qtmp,tmp)!=NULL){
            return 1;
        }
    }
    return 0;

}

int checkParenthesis(char *query){
    char Stack[MAX]={""};
    int top=MAX-1;
    int l=strlen(query);
    int i;
    for(i=0;i<l;i++){
        if(query[i]=='('){
            push(Stack, &top, '(');
        }
        if(query[i]==')'){
            if(isEmpty(top)){return 0;}
            char c=pop(Stack,&top);
            if(c!='('){return 0;}
        }
    }
    if(!isEmpty(top)){return 0;}
    return 1;
}

char checkOps(char *query){
    int ops=0,wrds=0,l=strlen(query),i;
    query[l]=' ';
    query[l+1]=0;
    l++;
    char c=0;
    for(i=0;i<l;i++){
        if(isNum(query[i]) && query[i+1]==' '){wrds++;}
        if(isOperator(query[i]) && query[i]!='!'){ops++;c=query[i];}
        if(ops>wrds){return query[i];}
        if(wrds>ops+1){
            int j=i+1;
            while(j<l && !isOperator(query[j])){
                j++;
            }
            if(isOperator(query[j])){return query[j];}
            else {return 10;}
        }
    }
    if(ops==wrds && ops!=0){return c;}
    if(wrds-ops>1){return 10;}
    return 0;
}

int checkNot(char *query){
    char *tmp=(char*)malloc(MAX*sizeof(char));
    strcpy(tmp,query);
    int i,l=strlen(tmp);
    for(i=0;i<l;i++){
        if(tmp[i]=='!'){
            int j=i-1;
            if(i==0){j=0;}
            for(j;j<l-2;j++){
                tmp[j]=tmp[j+2];
            }
            tmp[l-2]=0;
            i--;
            l-=2;
        }
    }
    if(checkParenthesis(tmp) && !hasSkippedWord(tmp) && !checkOps(tmp)){return 0;}
    return 1;
}

void autoCorrect(struct bstNode *r, char *word){
    struct cor tmp;
    int dis=LD(toLowerString(r->name),toLowerString(word));
    if(dis<top3[2].dis){
        tmp.word=r->name;
        tmp.dis=dis;
        top3[2]=tmp;
        sortCor();
    }
    if(r->right!=NULL){autoCorrect(r->right,word);}
    if(r->left!=NULL){autoCorrect(r->left,word);}
    return;
}

int checkValid(char *query, int single){
    int l=strlen(query);
    if(single==1){
        if(hasSign(query,0)){return 1;}
        if(isSkippedWord(query)){return 2;}
        int cond=processQuery(query,1);
        if(cond==-1){return -1;}
        if(cond==3){
            SetColor(RED);printf("Error: No Files Found\nDid You Mean?\n");SetColor(WHITE);
            clearCor();
            autoCorrect(bstroot->right,query);
            int i;
            for(i=0;i<3;i++){
                printf("%d. %s\n",i+1,top3[i].word);
            }
            printf("Or Press 4 to Terminate...\n");
            i='0';
            while(!(i>='1'&& i<='4')){
                i=getch();
            }
            if(i=='4'){return 100;}
            SetColor(15);printf("--> ");SetColor(WHITE);
            printf("%s\n",top3[i-'1'].word);
            return i-'0'+10;
        }
    }
    if(single==0){
        if(!checkParenthesis(query)){return 1;}
        if(hasSkippedWord(query)){return 2;}
        char o=checkOps(query);
        if(isOperator(o)){return o;}
        if(checkNot(query)){return 3;}
        if(o==0){
            char *postfix=(char*)malloc(MAX*sizeof(char));
            postfix=infixToPostfix(query);
            int res=processQuery(postfix,0);
            if(res==0){return 9;}
            if(res==8){return 8;}
            if(res==7){return -1;}
            //return -1;
        }
    }
}

int isSingleWord(char *query){
    int l=strlen(query);
    int i;
    for(i=0;i<l;i++){
        if(query[i]==' '){return 0;}
    }
    return 1;
}

char *makeQueryStandard(char *query){
    char *out=(char*)malloc(MAX*sizeof(char));
    strcpy(out,query);
    strcat(out," | ");
    strcat(out,query);
    return out;
}

void divide(){
    int i;
    for(i=0;i<MAX;i++){
        printf("_");
    }
    printf("\n");
}

int main()
{
    char dir[MAX]={""};
    char *query=(char*)malloc(MAX*sizeof(char));
    int validDir=0;
    while(!validDir){
        printf("\n");
        SetColor(15);printf("Enter Base Directory--> ");SetColor(WHITE);
        gets(dir);
        validDir=walkDir(dir,1);
    }
    walkDir(dir,0);
    root=(struct node *)malloc(sizeof(struct node));
    bstroot=(struct bstNode *)malloc(sizeof(struct bstNode));
    root->next=NULL;
    cur=root;
    int i;
    for(i=0;i<numFiles;i++){
        strcpy(filesNames[i],fileName(pathes[i]));
        readFile(pathes[i]);
    }
    bstroot->left=NULL;
    bstroot->right=NULL;
    bstroot->name="";
    LLtoBST();
    char queries[MAX][MAX];int top=MAX-1;
    char altQueries[MAX][MAX];int altTop=MAX-1;
    int flag=1;int isSingle=0;
    while(1){
        SetColor(15);printf("Enter Your Query--> ");SetColor(WHITE);
        Qroot=(struct bstNode *)malloc(sizeof(struct bstNode));
        Qroot->next=NULL;
        Qcur=Qroot;
        gets(query);
        if(strlen(query)==0){continue;}
        if(strcmp("Terminate",query)==0){break;}
        if(strcmp("History",query)==0){
            if(isEmpty(top)){SetColor(RED);printf("Error: No History Found\n");SetColor(WHITE);continue;}
            char h='2';
            char *q=(char*)malloc(MAX*sizeof(char));
            while(h=='2' && !isEmpty(top)){
                strcpy(q,pop_s(queries,&top));
                push_s(altQueries,&altTop,q);
                SetColor(15);printf("--> ");SetColor(WHITE);
                printf("<%s>\n1. OK\n2. History\n", q);
                h='0';
                while(!(h=='1' || h=='2')){h=getch();if(h=='2' && !isEmpty(top)){divide();}}
            }
            while(!isEmpty(altTop)){
                char *tmp=(char*)malloc(MAX*sizeof(char));
                strcpy(tmp,pop_s(altQueries,&altTop));
                push_s(queries,&top,tmp);
            }
            if(h=='2'){SetColor(RED);printf("Error: No More History Found\n");SetColor(WHITE);divide();continue;}
            if(h=='1'){
                time_t T1=clock();
                if(isSingleWord(q)){int cond=checkValid(q,1);}
                else{int cond=checkValid(q,0);}
                time_t T2=clock();
                SetColor(15);printf("Time: %f sec\n", (float)(T2-T1)/CLOCKS_PER_SEC);SetColor(WHITE);
                divide();
            }
            continue;
        }
        time_t t1=clock();
        if(isSingleWord(query)){
            int cond=checkValid(query,1);
            if(cond==-1){push_s(queries,&top,query);}
            if(cond==1){SetColor(RED);printf("Error: Your Search Shouldn't Contain Signs\n");SetColor(WHITE);continue;}
            if(cond==2){SetColor(RED);printf("Error: Your Search Is In Skipped Words\n");SetColor(WHITE);continue;}
            if(cond>=11 && cond<=13){
                t1=clock();
                int cnd=checkValid(top3[cond-11].word,1);
                if(cnd==-1){push_s(queries,&top,top3[cond-11].word);}
            }
            if(cond==100){break;}
        }
        else {
            int cond=checkValid(query,0);
            if(cond==1){SetColor(RED);printf("Syntax Error: Invalid Use Of Parenthesis\n");SetColor(WHITE);continue;}
            if(cond==2){SetColor(RED);printf("Error: Your Search Includes Skipped Words\n");SetColor(WHITE);continue;}
            if(cond==3){SetColor(RED);printf("Error: Invalid Use Of Operator <!>\n");SetColor(WHITE);continue;}
            if(cond=='|' || cond=='&' || cond=='^'){SetColor(RED);printf("Syntax Error: Invalid Use Of Operator <%c>\n",cond);SetColor(WHITE);continue;}
            if(cond==10){SetColor(RED);printf("Syntax Error: Too Few Operators\n");SetColor(WHITE);continue;}
            if(cond==8){SetColor(RED);printf("No Files Found\n");SetColor(WHITE);continue;}
            if(cond==9){SetColor(RED);printf("Syntax Error!\n");SetColor(WHITE);continue;}
            if(cond==-1){push_s(queries,&top,query);}
        }
        time_t t2=clock();
        SetColor(15);printf("Time: %f sec\n", (float)(t2-t1)/CLOCKS_PER_SEC);SetColor(WHITE);
        divide();
    }
    getch();
    return 0;
}
