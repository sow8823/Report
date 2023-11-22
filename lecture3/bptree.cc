#include "bptree.h"
#include <vector>
#include <sys/time.h>


struct timeval
cur_time(void)
{
	struct timeval t;
	gettimeofday(&t, NULL);
	return t;
}

int cut(int length) {
  if (length % 2 == 0)
    return length / 2;
  else
    return length / 2 + 1;
}

void
print_tree_core(NODE *n)
{
  if (n == NULL) return;
	printf("["); 
	for (int i = 0; i < n->nkey; i++) {
		if (!n->isLeaf) print_tree_core(n->chi[i]); 
		printf("%d", n->key[i]); 
		if (i != n->nkey-1 && n->isLeaf) putchar(' ');
	}
	if (!n->isLeaf) print_tree_core(n->chi[n->nkey]);
	printf("]");
}

void
print_tree(NODE *node)
{
	print_tree_core(node);
	printf("\n"); fflush(stdout);
}

//
//
//

DATA *
alloc_data(int key)
{
  DATA *data;
  if (!(data = (DATA *)calloc(1, sizeof(DATA)))) ERR;
  data->key = key;
  data->val = key;
  data->next = NULL;

  return data;
}

NODE * 
alloc_node() {
	NODE *node;
	if (!(node = (NODE *)calloc(1, sizeof(NODE)))) ERR;
  node->isLeaf = false;
  node->nkey = 0;
  node->parent = NULL;
  node->next = NULL;
  return node;
}

NODE *
alloc_leaf()
{
	NODE *leaf = alloc_node();
  leaf->isLeaf = true;

	return leaf;
}

//
//
//

NODE *
find_leaf(NODE *node, int key)
{
	int kid;

	if (node->isLeaf) return node;
	for (kid = 0; kid < node->nkey; kid++) {
		if (key < node->key[kid]) break;
	}

	return find_leaf(node->chi[kid], key);
}

// 同一の値があるか探す
DATA *
find_data(NODE *node, int key, NODE **leaf_out) {
  if (node == NULL) {
    if (leaf_out != NULL) *leaf_out = NULL;
    // node が NULL なら data も無い
    return NULL;
  }
  int i = 0;
  NODE *leaf = NULL;

  leaf = find_leaf(node, key);

  for (i=0; i < leaf->nkey; i++) {
    if (leaf->key[i] == key) break;
    if (leaf_out != NULL) *leaf_out = leaf;
  }
  // nkey まで探して無ければNULL
  if (i == leaf->nkey) return NULL;
  return (DATA *)leaf->chi[i];
}

NODE *
insert_in_leaf(NODE *leaf, int key, DATA *data)
{
  int i;
  int insertion;
  insertion = 0;
  while(insertion < leaf->nkey && leaf->key[insertion] < key) {
    insertion++;
  }
  for (i = leaf->nkey; i > insertion; i--) {
    leaf->key[i] = leaf->key[i-1];
    leaf->chi[i] = leaf->chi[i-1];
  }
  leaf->key[insertion] = key;
  leaf->chi[insertion] = (NODE *)data;
  leaf->nkey++;
	return leaf;
}

// node に要素追加。rootはそのまま返す
// nkey < N-1 であること
NODE *
insert_in_node(NODE *node, NODE *n, int left_index, int key, NODE *right)
{
  int i;
  for (i = n->nkey; i > left_index; i--) {
    n->chi[i+1] = n->chi[i];
    n->key[i] = n->key[i-1];
  }
  n->chi[left_index+1] = right;
  n->key[left_index] = key;
  n->nkey++;
  return node;
}

// 親を分割して要素追加して、新しいrootを返す
NODE * 
insert_in_node_after_splitting(NODE *node, NODE *old_node, int left_index, int key, NODE *right) {
  int i, j, split, k_prime;
  NODE *new_node, *child;
  // 一つ大きいサイズで確保
  int *temp_keys;
  if (!(temp_keys = (int *)malloc(N * sizeof(int)))) ERR;
  NODE **temp_childlen;
  if (!(temp_childlen = (NODE **)malloc((N+1) * sizeof(NODE *)))) ERR;

  // 一時領域にコピー
  for (i=0, j=0; i < old_node->nkey + 1; i++, j++) {
    if (j == left_index + 1) j++;
    temp_childlen[j] = old_node->chi[i];
  }

  for (i=0, j=0; i < old_node->nkey; i++, j++) {
    if (j == left_index) j++;
    temp_keys[j] = old_node->key[i];
  }

  temp_childlen[left_index + 1] = right;
  temp_keys[left_index] = key;

  // 分割場所
  split = cut(N);
  new_node = alloc_node();
  // いったん初期化
  old_node->nkey = 0;
  // 左側に入れる
  for (i=0; i < split; i++) {
    old_node->chi[i] = temp_childlen[i];
    old_node->key[i] = temp_keys[i];
    old_node->nkey++;
  }
  // 左側の端っこを保存
  old_node->chi[i] = temp_childlen[i];
  k_prime = temp_keys[split];
  // 右側に入れる
  for(++i, j=0; i < N; i++, j++) {
    new_node->chi[j] = temp_childlen[i];
    new_node->key[j] = temp_keys[i];
    new_node->nkey++;
  }
  // 右側の端っこを保存
  new_node->chi[j] = temp_childlen[i];
  // 解放
  free(temp_childlen);
  free(temp_keys);

  // 親を共有
  new_node->parent = old_node->parent;
  // 配下に新しい親を設定
  for(i=0; i <= new_node->nkey; i++) {
    child = new_node->chi[i];
    child->parent = new_node;
  }
  // 親に要素追加して、leftとrightをぶら下げて、新しいrootを返す
  return insert_in_parent(node, old_node, k_prime, new_node);
}

// 親NODEに要素追加して、leftとrightをぶら下げる
// root が変わる可能性があるので NODE * を返す
NODE * 
insert_in_parent(NODE *root, NODE *left, int key, NODE *right)
{
  NODE *parent;
  int left_index;
  // 暫定のroot
  parent = left->parent;

  // 親が無いなら
  if (parent == NULL) {
    std::cout << "parent is null" << std::endl;
    // 新しく作ったparentをrootとして返す
    return insert_in_new_root(left, key, right);
  }
  std::cout << "parent is not null" << std::endl;
  // 今回追加したいキーが親のどこにあるか探す
  left_index = 0;
  while (left_index < parent->nkey && parent->chi[left_index] != left) {
    left_index++;
  }
  std::cout << "left_index: " << left_index << std::endl;
  if (parent->nkey < N-1) {
    std::cout << "parent->nkey < N-1" << std::endl;
    // 親に要素追加してleftとrightをぶら下げる
    // rootはそのまま
    return insert_in_node(root, parent, left_index, key, right);
  }
  // 親をsplitして要素追加して、新しいrootを返す
  return insert_in_node_after_splitting(root, parent, left_index, key, right);
}

// leafを分割して要素追加して、新しいrootを返す
NODE *
insert_in_leaf_after_splitting(NODE *root, NODE *leaf, int key, DATA *data) {
  NODE *new_leaf;
  int *temp_keys;
  void **temp_data;
  int insertion, split, new_key, i, j;

  // 分割先
  new_leaf = alloc_leaf();

  // 一時領域確保
  if (!(temp_data = (void **)malloc(N * sizeof(void *)))) ERR;
  if (!(temp_keys = (int *)malloc(N * sizeof(int)))) ERR;

  // 新規要素が入る場所を探す
  insertion = 0;
  while (insertion < N -1 && leaf->key[insertion] < key) {
    insertion++;
  }
  // ずらして
  for (i=0, j=0; i < leaf->nkey; i++, j++) {
    if (j == insertion) j++;
    temp_keys[j] = leaf->key[i];
    temp_data[j] = leaf->chi[i];
  }
  // 入れる
  temp_keys[insertion] = key;
  temp_data[insertion] = (NODE *)data;

  // いったん初期化して
  leaf->nkey = 0;

  // 分割場所
  split = cut(N-1);

  // 左側に入れる
  for (i=0; i < split; i++) {
    leaf->chi[i] = (NODE *)temp_data[i];
    leaf->key[i] = temp_keys[i];
    leaf->nkey++;
  }

  // 右側に入れる
  for (i=split, j=0; i < N; i++, j++) {
    new_leaf->chi[j] = (NODE *)temp_data[i];
    new_leaf->key[j] = temp_keys[i];
    new_leaf->nkey++;
  }
  // 領域解放
  free(temp_data);
  free(temp_keys);

  // B+ なので連結リストにする
  new_leaf->chi[N-1] = leaf->chi[N-1];
  leaf->chi[N-1] = new_leaf;

  // 余った領域はNULLで埋める
  for (i = leaf->nkey; i < N-1; i++) {
    leaf->chi[i] = NULL;
  }
  for (i = new_leaf->nkey; i < N-1; i++) {
    new_leaf->chi[i] = NULL;
  }
  // 親へのポインタを設定
  new_leaf->parent = leaf->parent;
  // 右側の先頭キーを親に追加
  new_key = new_leaf->key[0];

  // 親にleftとrightをぶら下げる
  return insert_in_parent(root, leaf, new_key, new_leaf);
}

//
//
//
NODE *insert_in_new_root(NODE *left, int key, NODE *right) {
  NODE *root;
  root = alloc_node();
  root->key[0] = key;
  root->chi[0] = left;
  root->chi[1] = right;
  root->nkey++;
  root->parent = NULL;
  left->parent = root;
  right->parent = root;
  return root;
}

NODE *init_new_tree(int key, DATA *data) {
  NODE *root;
  root = alloc_leaf();
  root->key[0] = key;
  root->chi[0] = (NODE *)data;
  root->chi[N -1] = NULL;
  root->parent = NULL;

  root->nkey++;
  return root;
}

NODE * 
insert(NODE *root, int key)
{
  DATA *data;
	NODE *leaf;
  // すでにあるか探す
  data = find_data(root, key, NULL);
  if (data != NULL) {
    // 上書きするふり
    data->val = key;
    return root;
  }
  // なければ新規作成
  data = alloc_data(key);
  // Root が NULL なら新規作成
	if (root == NULL) {
    return init_new_tree(key, data);
	}

  leaf = find_leaf(root, key);
	if (leaf->nkey < (N-1)) {
		leaf = insert_in_leaf(leaf, key, data);
    return root;
	}

  return insert_in_leaf_after_splitting(root, leaf, key, data);
}

int 
interactive()
{
  int key;
  std::cout << "Key: ";
  std::cin >> key;

  return key;
}

int
main(int argc, char *argv[])
{
  struct timeval begin, end;

  NODE *root;
  root = NULL;

	printf("-----Insert-----\n");
	begin = cur_time();
  while (true) {
    
		root = insert(root, interactive());
    print_tree(root);
  }
	end = cur_time();

	return 0;
}
