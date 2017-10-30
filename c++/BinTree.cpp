/*
  insert()				- Inserts node into tree							O(H)
  size()				- Calculate size of tree (how many elements)		O(n)
  maxDepth()			- Finds maximum depths of tree	(Maximum number of nodes in a certain way. If need max num of edges - change func code). Time-complexity: O(n)
  CheckIfBST()			- Tests if tree is BinarySearchTree					O(n)
  FindKey()				- Finds node with data = key						O(H)
  FindMin()				- Finds minimum element in tree						O(H)
  FindMax()				- Finds maximum element in tree						O(H)
  DeleteNode()			- Deletes node and rearrange tree if necessary		O(H)
  FindSuccessor()		- Finds node with next greater value to argument	O(H)
  FindPredecessor()		- Finds node with previous value to argument		O(H)
  CompareBinaryTrees()	- Check if two binary trees are identical.			O(n)
  HasPathSum()			- checks if there is a path in tree that sum of nodes equal to argument		O(n)
  printPaths()			- print all pathes from root to all leaves			O(n)
  mirror()				- swaps left and right pointers at each node		O(n)
  doubleTree()			- for each node creates duplicate and insert at the left of original node		O(n)
  
*/

#include <stdio.h>

struct node {
  int data;
  struct node *left;
  struct node *right;
};

// returns pointer to new node
struct node *NewNode(int data) {
  struct node *node = new(struct node);
  node->data = data;
  node->left = NULL;
  node->right = NULL;
  return node;
}

node *insert(node *node, int data) {
	if (node == NULL) return NewNode(data);
	else if (data <= node->data) node->left = insert(node->left, data);
	else node->right = insert(node->right, data);
	return node;
}

// print tree in order
void printTree(struct node *node) {
  if (node == NULL) return;
  printTree(node->left);
  printf("%d ", node->data);
  printTree(node->right);
}

int size(struct node *node) {
  if (node == NULL) return 0;
  return 1+size(node->left)+size(node->right);
}
int maxDepth(struct node *node) {
  if (node == NULL) return 0;		// if return 0 - func will return max num of nodes in path. If need return max num of edges - change to "return -1"
  int ldepth = maxDepth(node->left);
  int rdepth = maxDepth(node->right);
  if (ldepth > rdepth) return 1 + ldepth;
  else return 1 + rdepth;
}

int CheckIfBST_Aux(node *n, int LoMargin, int HiMargin) {
	if (n == NULL) return 1;
	if ((n->data > LoMargin)
		&&(n->data <= HiMargin)
		&&(CheckIfBST_Aux(n->left, LoMargin, n->data))
		&&(CheckIfBST_Aux(n->right, n->data, HiMargin)))
		return 1;
	else	
		return 0;
}
int CheckIfBST(node *n) {
	return CheckIfBST_Aux(n, -30e3, 30e3);
}

node *FindKey(node *n, int key) {
	if (n==NULL) return NULL;
	else if (key == n->data) return n;
	else if (key < n->data) return FindKey(n->left, key);
	else /* if (key > n->data) */ return FindKey(n->right, key);
}

node *FindMin(node *n) {
	if (n==NULL) return NULL;
	while (n->left != NULL) {
		n = n->left;
	}
	return n;
}
node *FindMax(node *n) {
	if (n==NULL) return NULL;
	while (n->right != NULL) {
		n = n->right;
	}
	return n;
}

node *DeleteNode(node *n, int key) {
	node *temp;
	if (n == NULL) return NULL;
	else if (key < n->data) n->left = DeleteNode(n->left, key);
	else if (key > n->data) n->right = DeleteNode(n->right, key);
	else { // it's Node to be deleted
		if ((n->left==NULL)&&(n->right==NULL)) {
			delete n;
			n = NULL;
		} else if (n->left == NULL) {
			temp = n;
			n = n->right;
			delete temp;
		} else if (n->right == NULL) {
			temp = n;
			n = n->left;
			delete temp;
		} else {		// Instead of deletion - move to this position another deeper node, and delete that deeper node in next iteration. It will be easier because in yet another iteration this node will have only 1 subtree or be leaf
			temp = FindMin(n->right);	// The proper node to became new subroot is the node that keeps property of BST (left subtree smaller, right - bigger)
			n->data = temp->data;		// It can be maximum form left subtree, or minimum from right subtree. In this example minimun from right is used
			n->right = DeleteNode(n->right, temp->data);
		}
	}
	return n;
}

node *FindSuccessor(node *root, int key) {
	node *current;
	node *parent;
	
	if (root==NULL) return NULL;
	current = FindKey(root, key);
	if (current==NULL) return NULL;
	else if (current->right != NULL) {		// node has right subtree. next gretar value to node->data is the minimum value of right subtree 
		return FindMin(current->right);
	} else {								// does not have right subtree. next greater value to node->data is in closest parent with left-relation
		current = root;
		parent = NULL;
		while (current->data != key) {
			if (key <= current->data) {
				parent = current;
				current = current->left;	
			} else current = current->right;
		}
		return parent;
	}
}

node *FindPredecessor(node *root, int key) {
	node *current;
	node *parent;
	
	if (root==NULL) return NULL;
	current = FindKey(root, key);
	if (current==NULL) return NULL;
	else if (current->left != NULL) {		// node has left subtree. previous value to node->data is the maximum value of left subtree 
		return FindMax(current->left);
	} else {								// does not have left subtree. previous value to node->data is in closest parent with right-relation
		current = root;
		parent = NULL;
		while (current->data != key) {
			if (key <= current->data) {
				current = current->left;	
			} else {
				parent = current;
				current = current->right;
			}
		}
		return parent;
	}
}

int CompareBinaryTrees(node *tree1, node *tree2) {
	if ((tree1 == NULL)&&(tree2==NULL)) return 1;
	else if ((tree1!=NULL)&&(tree2!=NULL)) {
	  if (tree1->data != tree2->data) return 0;
	  return CompareBinaryTrees(tree1->left, tree2->left)*CompareBinaryTrees(tree1->right, tree2->right);
	}
	else return 0;  // One empty, another not empty
}

int HasPathSum(node *node, int sum) {
	if (node == NULL) return (sum == 0);
	return 	HasPathSum(node->left, sum-node->data)||HasPathSum(node->right, sum-node->data);
}

void printPaths_Aux(node *node, int *path, int cnt) {
	if (node==NULL) return;	
	path[cnt++]=node->data;
	if ((node->left == NULL)&&(node->right==NULL)) {
		for(int i=0; i<cnt; i++) printf("%4d", path[i]);
		printf("\n");
	} else {
		printPaths_Aux(node->left, path, cnt);
		printPaths_Aux(node->right, path, cnt);
	}
}
void printPaths(node *root) {
	int path[100];
	printPaths_Aux(root, path, 0);
}
void mirror(struct node *node) {
  struct node *tmp;
  if (node==NULL) return;
  tmp = node->left;
  node->left = node->right;
  node->right = tmp;
  mirror(node->left);
  mirror(node->right);  
}
void doubleTree(struct node *node) {
  struct node *tmp;
  if (node==NULL) return;
  doubleTree(node->left);
  doubleTree(node->right);
  tmp = node->left;
  node->left = NewNode(node->data);
  node->left->left = tmp;
}

void main() {
	node *tree1;
	node *tree2;
	node *tree3;
	node *tree4;
	
	node *n;
	node *root;
	
	tree1 = NewNode(8);
	tree1->left = NewNode(6);
	tree1->left->left = NewNode(4);
	
	tree2 = NewNode(10);
	tree2->left = NewNode(5);
	tree2->right = NewNode(16);
	tree2->left->left = NewNode(4);
	tree2->left->right = NewNode(7);
	tree2->left->left->left = NewNode(1);
	tree2->left->right->right = NewNode(11);
	
	tree3 = NewNode(7);
	tree3->left = NewNode(4);
	tree3->right = NewNode(9);
	tree3->left->left = NewNode(1);
	tree3->left->right = NewNode(6);
	
	tree4 = NewNode(5);
	tree4->left = NewNode(1);
	tree4->right = NewNode(8);
	tree4->right->left = NewNode(9);
	tree4->right->right = NewNode(12);
	
	printf("Print tree in order: ");
	printTree(tree1);
	printf("Check if tree is BST : %d\n", CheckIfBST(tree1));
	printf("Print tree in order :");
	printTree(tree2);
	printf("Check if tree is BST : %d\n", CheckIfBST(tree2));
	printf("Print tree in order: ");
	printTree(tree2);
	printf("Check if tree is BST : %d\n", CheckIfBST(tree3));
	printf("Print tree in order: ");
	printTree(tree4);
	printf("Check if tree is BST : %d\n", CheckIfBST(tree4));

	root = NewNode(12);					//					12
	root->left = NewNode(5);			//
	root->right = NewNode(15);			//			5					15
	root->left->left = NewNode(3);		//	
	root->left->right = NewNode(7);		//		3		7			13		17	
	root->right->left = NewNode(13);	//
	root->right->right = NewNode(17);	//	1				9
	root->left->left->left = NewNode(1);
	root->left->right->right = NewNode(9);

	
	printf("\nInitial tree\n");
	printTree(root);
	printf("\nSize of tree is %d elements\n", size(root));
	printf("Depth of tree is %d\n", maxDepth(root));
	printf("Minimum value in tree is %d\n", FindMin(root)->data);
	printf("Maximum value in tree is %d\n", FindMax(root)->data);
	printf("Find element %d: %d\n", 12, FindKey(root, 12)->data);
	/*
	printf("Find element %d:", 22);
	n = FindKey(root, 22); if (n != NULL) printf("%d\n", n->data);
	else printf("not found\n");
	*/
	printf("\n\nFinding successor\n");
	for (int key=1; key<18; key++) {
	  n = FindSuccessor(root, key); 
	  if (n!=NULL) printf("Successor of %d is %d\n", key, n->data);
	}  
	printf("\n\nFinding predecessor\n");
	for (int key=1; key<18; key++) {
	  n = FindPredecessor(root, key); 
	  if (n!=NULL) printf("Predecessor of %d is %d\n", key, n->data);
	}  
	printf("\nTree before node deletion\n");
	printTree(root);
	root = DeleteNode(root, 15);
	printf("\nTree after node deletion\n");
	printTree(root);

	node *t1 = NULL; node *t2 = NULL;
	t1 = insert(t1, 10); t2 = insert(t2, 10);	// 				10
	t1 = insert(t1, 5); t2 = insert(t2, 5);		// 		5				15
	t1 = insert(t1, 1); t2 = insert(t2, 1); 	//	1				11
	t1 = insert(t1, 3); t2 = insert(t2, 3); 	//		3				13
	t1 = insert(t1, 15); t2 = insert(t2, 15); 
	t1 = insert(t1, 11); t2 = insert(t2, 11); 
	t1 = insert(t1, 13); t2 = insert(t2, 13); 
	printf("\n\nComparing trees. Result = %d\n", CompareBinaryTrees(t1, t2));
	
	int sum = 49;
	printf("\n\nFind if there is path with sum=%d. Result = %d\n", sum, HasPathSum(t1, sum));

	printf("All pathes of tree\n");
	printPaths(t1);
}