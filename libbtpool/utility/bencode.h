#ifndef BENCODE_H_201409222238
#define BENCODE_H_201409222238
#include <stddef.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

#define CRYPT_OK 0
#define CRYPT_INVALID_ROUNDS 4
#define CRYPT_BUFFER_OVERFLOW 6
#define CRYPT_INVALID_PACKET 7
#define CRYPT_MEM 13
#define CRYPT_INVALID_ARG 16
#define CRYPT_FUNC_FAILED 101

#define BNODE_INT 'i'
#define BNODE_DICT 'd'
#define BNODE_LIST 'l'
#define BNODE_BUF 'b'

// when modify any bnode, all nodes with type 'd' and 'l' are not reliable
// rawLen,rawText,num,content only reliable after bencode_decode
typedef struct _BNode
{
    char rawtype;	//'i','d','l','b'  0x80
    int  rawLen;    //rawLen,rawText: when modify bnode,
    const char *rawText; //these two are not reliable.

    //if type is 'i', means number itself
    //if type is 'd', means count of key=value pairs
    //if type is 'l', means count of list node
    //if type is 'b', means valid length of string
    int64_t num;

    //only valid when type is 'b'
    //valid length is 'num',no '\0' at end.
    const char *content;

    struct _BNode *sub;
    struct _BNode *next;

#ifdef __cplusplus
    char type()
    {
        //'i','d','l','b'
        return this->rawtype&0x7F;
    }
#endif
} BNode,*PBNode;

//TODO 大数字容易溢出
void bencode_make_child(PBNode parent,PBNode sub);
void bencode_make_brother(PBNode brother,PBNode next);

PBNode bencode_free_node(PBNode node,PBNode parent,PBNode prev);
void bencode_free_node_tree(PBNode nodeTree);
void bencode_free_node_solo(PBNode nodeTree);  //不处理链表和层级关系,直接删node.
int	bencode_decode(const void *in,  size_t len, PBNode *root); //返回处理过的字节

int bencode_create_m(char type,PBNode *created); //used for 'l','d'
int bencode_create_i(int64_t num,PBNode *created); //used for 'i'
int bencode_create_b(const void *in, size_t len, PBNode *created); //used for 'b'uff

//please give valid length of 'out' by 'outlen'.
//if CRYPT_BUFFER_OVERFLOW returned, 'outlen' should help.
int bencode_encode(PBNode root, char *out, size_t *outlen);

int bencode_find_value(PBNode dict, const void *key, size_t keyLen, PBNode *found);



#ifdef __cplusplus
}
#endif
#endif
