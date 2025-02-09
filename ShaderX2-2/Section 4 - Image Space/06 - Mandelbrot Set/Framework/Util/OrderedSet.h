                  /*--.          .-"-. 
                 /   ._.        / � ` \
                 \_  (__\       \_�v�_/
                 //   \\        //   \\
                ((     ))      ((     ))
 ��������������--""---""--����--""---""--��������������������������������������������
 �                 |||            |||                                               �
 �  Author's note:  |              |                                                �
 �                                                                                  �
 �  This file is part of work done by Emil Persson, aka Humus.                      �
 �  You're free to use the code in any way you like, modified, unmodified           �
 �  or cut'n'pasted into your own work. But for the good of the whole               �
 �  programming community I will ask you to as long as possible and to the          �
 �  extent you find suitable to follow these points:                                �
 �   * If you use this file and it's contents unmodified, please leave this note.   �
 �   * If you use a major part of the file in your app, please credit the author(s) �
 �     in one way or the other as you find suitable, for instance a small notice    �
 �     in the readme file may do, or perhaps a link to the authors site.            �
 �   * Share your work and ideas too as much as you can.                            �
 �    _______                                                                       �
 �  /` _____ `\;,    Humus                                                          �
 � (__(^===^)__)';,  emiper-8@student.luth.se                         ___           �
 �   /  :::  \   ,;  ICQ #47010716                                  /^   ^\         �
 �  |   :::   | ,;'                                                ( �   � )        �
 ���'._______.'`����������������������������������������������--�oOo--(_)--oOo�--��*/

#ifndef _ORDEREDSET_H_
#define _ORDEREDSET_H_

/* 
	This file implements an ordered set.
	It's a data structure that allows you to insert, delete, search items in O(ln n) time.
	This is done with a so called Red-Black binary search tree, which is a binary tree which
	is garantueed to be balanced such that no path from the root to a leaf is longer than twice
	the length of the shortest path. This while maintaining the fast O(ln n) inserting,
	deleting and searching properties.

	For a explanation of the algoritm used, check the book "Introduction To Algorithms"
	(Cormen, Thomas H. / Leiserson, Charles E . / Rivest, Ronald L)

	Visual illustration:
	http://www.geocities.com/SiliconValley/Network/1854/Rbt.html
*/


enum COLOR { RED, BLACK };

template <class ARG_TYPE>
class Node {
public:
	ARG_TYPE object;

	Node *parent;
	Node *left;
	Node *right;

	COLOR color;
};


template <class ARG_TYPE>
class OrderedSet {
protected:
	Node <ARG_TYPE> *root, *curr, *null;
	int size;

	Node <ARG_TYPE> *treeInsert(const ARG_TYPE &object);
	void treeRemove(Node <ARG_TYPE> *node);
	void treeRemoveFixColors(Node <ARG_TYPE> *node);
	void leftRotate(Node <ARG_TYPE> *node);
	void rightRotate(Node <ARG_TYPE> *node);

	void clearNode(Node <ARG_TYPE> *node);

	Node <ARG_TYPE> *minNode(Node <ARG_TYPE> *node){
		while (node->left != null) node = node->left;
		return node;
	}

	Node <ARG_TYPE> *maxNode(Node <ARG_TYPE> *node){
		while (node->right != null) node = node->right;
		return node;
	}

	Node <ARG_TYPE> *nextNode(Node <ARG_TYPE> *node){
		if (node->right != null) return minNode(node->right);

		Node <ARG_TYPE> *pNode = node->parent;
		while (pNode != null && node == pNode->right){
			node  = pNode;
			pNode = pNode->parent;
		}

		return pNode;
	}

	Node <ARG_TYPE> *prevNode(Node <ARG_TYPE> *node){
		if (node->left != null) return maxNode(node->left);

		Node <ARG_TYPE> *pNode = node->parent;
		while (pNode != null && node == pNode->left){
			node  = pNode;
			pNode = pNode->parent;
		}

		return pNode;
	}

	Node <ARG_TYPE> *findNode(const ARG_TYPE &object){
		Node <ARG_TYPE> *node = root;
		while (node != null && object != node->object){
			node = (object < node->object)? node->left : node->right;
		}
		return node;
	}
public:
	OrderedSet(){
		size = 0;
		null = new Node <ARG_TYPE>;
		null->left   = NULL;
		null->right  = NULL;
		null->parent = NULL;
		null->color = BLACK;
		curr = root = null;
	}
	
	~OrderedSet(){
		clearNode(root);
		delete null;
	}

	void clear(){
		clearNode(root);
		size = 0;
		curr = root = null;
	}

	int getSize() const { return size; }

	const ARG_TYPE &getCurrent() const { return curr->object; }

	bool isInSet(const ARG_TYPE &object){ return findNode(object) != null; }

	bool goToFirst(){
		if (root == null) return false;
		curr = minNode(root);
		return true;
	}

	bool goToLast(){
		if (root == null) return false;
		curr = maxNode(root);
		return true;
	}

	bool goToObject(const ARG_TYPE &object){
		return (curr = findNode(object)) != null;
	}

	bool next(){
		if (curr == null) return false;
		return (curr = nextNode(curr)) != null;
	}

	bool prev(){
		if (curr == null) return false;
		return (curr = prevNode(curr)) != null;
	}

	void insert(const ARG_TYPE &object);
	bool remove(const ARG_TYPE &object);

};


/* ------------------------------------------------------------------------------- */


template <class ARG_TYPE>
void OrderedSet <ARG_TYPE>::clearNode(Node <ARG_TYPE> *node){
	if (node != null){
		clearNode(node->left);
		clearNode(node->right);
		delete node;
	}
}

template <class ARG_TYPE>
void OrderedSet <ARG_TYPE>::insert(const ARG_TYPE &object){
	Node <ARG_TYPE> *x = treeInsert(object);
	size++;

	x->color = RED;

	while (x != root && x->parent->color == RED){
		if (x->parent == x->parent->parent->left){
			Node <ARG_TYPE> *y = x->parent->parent->right;
			if (y->color == RED){
				x->parent->color = BLACK;
				y->color = BLACK;
				x->parent->parent->color = RED;
				x = x->parent->parent;
			} else {
				if (x == x->parent->right){
					x = x->parent;
					leftRotate(x);
				}
				x->parent->color = BLACK;
				x->parent->parent->color = RED;
				rightRotate(x->parent->parent);
			}
		} else {
			Node <ARG_TYPE> *y = x->parent->parent->left;
			if (y->color == RED){
				x->parent->color = BLACK;
				y->color = BLACK;
				x->parent->parent->color = RED;
				x = x->parent->parent;
			} else {
				if (x == x->parent->left){
					x = x->parent;
					rightRotate(x);
				}
				x->parent->color = BLACK;
				x->parent->parent->color = RED;
				leftRotate(x->parent->parent);
			}
		}
	}

	root->color = BLACK;
}

template <class ARG_TYPE>
bool OrderedSet <ARG_TYPE>::remove(const ARG_TYPE &object){
	Node <ARG_TYPE> *node = findNode(object);
	if (node == null) return false;
	treeRemove(node);
	size--;
	return true;
}

template <class ARG_TYPE>
Node <ARG_TYPE> *OrderedSet <ARG_TYPE>::treeInsert(const ARG_TYPE &object){
	Node <ARG_TYPE> *currNode, *lastNode, *newNode;

	lastNode = null;
	currNode = root;

	while (currNode != null){
		lastNode = currNode;
		currNode = (object < currNode->object)? currNode->left : currNode->right;
	}

	newNode = new Node <ARG_TYPE>;
	newNode->left  = null;
	newNode->right = null;
	newNode->object = object;

	newNode->parent = lastNode;
	if (lastNode == null){
		root = newNode;
	} else {
		if (newNode->object < lastNode->object){
			lastNode->left = newNode;
		} else {
			lastNode->right = newNode;
		}
	}

	return newNode;
}

template <class ARG_TYPE>
void OrderedSet <ARG_TYPE>::treeRemove(Node <ARG_TYPE> *node){
	Node <ARG_TYPE> *x, *y;

	y = (node->left == null || node->right == null)? node : nextNode(node);

	x = (y->left != null)? y->left : y->right;

	x->parent = y->parent;
	if (y->parent == null){
		root = x;
	} else {
		if (y == y->parent->left){
			y->parent->left = x;
		} else {
			y->parent->right = x;
		}
	}
	if (y != node) node->object = y->object;
	if (y->color == BLACK) treeRemoveFixColors(x);

	delete y;
}

template <class ARG_TYPE>
void OrderedSet <ARG_TYPE>::treeRemoveFixColors(Node <ARG_TYPE> *node){
	Node <ARG_TYPE> *w;

	while (node != root && node->color == BLACK){
		if (node == node->parent->left){
			w = node->parent->right;
			if (w->color == RED){
				w->color = BLACK;
				node->parent->color = RED;
				leftRotate(node->parent);
				w = node->parent->right;
			}
			if (w->left->color == BLACK && w->right->color == BLACK){
				w->color = RED;
				node = node->parent;
			} else {
				if (w->right->color == BLACK){
					w->left->color = BLACK;
					w->color = RED;
					rightRotate(w);
					w = node->parent->right;
				}
				w->color = node->parent->color;
				node->parent->color = BLACK;
				w->right->color = BLACK;
				leftRotate(node->parent);
				node = root;
			}
		} else {
			w = node->parent->left;
			if (w->color == RED){
				w->color = BLACK;
				node->parent->color = RED;
				rightRotate(node->parent);
				w = node->parent->left;
			}
			if (w->left->color == BLACK && w->right->color == BLACK){
				w->color = RED;
				node = node->parent;
			} else {
				if (w->left->color == BLACK){
					w->right->color = BLACK;
					w->color = RED;
					leftRotate(w);
					w = node->parent->left;
				}
				w->color = node->parent->color;
				node->parent->color = BLACK;
				w->left->color = BLACK;
				rightRotate(node->parent);
				node = root;
			}
		}
	}
	node->color = BLACK;
}


template <class ARG_TYPE>
void OrderedSet <ARG_TYPE>::leftRotate(Node <ARG_TYPE> *node){
	Node <ARG_TYPE> *y = node->right;

	node->right = y->left;
	if (y->left != null) y->left->parent = node;

	y->parent = node->parent;
	if (node->parent == null){
		root = y;
	} else {
		if (node == node->parent->left){
			node->parent->left = y;
		} else {
			node->parent->right = y;
		}
	}
	y->left = node;
	node->parent = y;
}

template <class ARG_TYPE>
void OrderedSet <ARG_TYPE>::rightRotate(Node <ARG_TYPE> *node){
	Node <ARG_TYPE> *y = node->left;

	node->left = y->right;
	if (y->right != null) y->right->parent = node;

	y->parent = node->parent;
	if (node->parent == null){
		root = y;
	} else {
		if (node == node->parent->right){
			node->parent->right = y;
		} else {
			node->parent->left = y;
		}
	}
	y->right = node;
	node->parent = y;
}


#endif // _ORDEREDLIST_H_
