/*
* Copyright (c) 2020 Divyansh Vinayak
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

#ifndef DYNAMIC_SEGMENT_TREE_HPP
#define DYNAMIC_SEGMENT_TREE_HPP

#include <cstddef>

namespace __rb_tree
{
	template <typename T>
	class dynamic_segment_tree
	{
		enum Color
		{
			RED,
			BLACK
		};

		struct Node
		{
			T data, sum, lazy;
			size_t size;
			Color color;
			Node *left, *right, *parent;
		};

		Node *root, *null;

		Node *Search(size_t pos)
		{
			Node *node = root;
			size_t idx = node->left->size + 1;
			Propagate(node);
			while (idx != pos)
			{
				if (pos < idx)
				{
					node = node->left;
				}
				else
				{
					pos -= idx;
					node = node->right;
				}
				idx = node->left->size + 1;
				Propagate(node);
			}
			return node;
		}

		void RotateLeft(Node *node)
		{
			Node *right = node->right;
			Propagate(right);
			Propagate(node->left);
			Propagate(right->left);
			node->right = right->left;
			if (right->left != null)
			{
				right->left->parent = node;
			}
			right->parent = node->parent;
			if (node == root)
			{
				root = right;
			}
			else if (node == node->parent->left)
			{
				node->parent->left = right;
			}
			else
			{
				node->parent->right = right;
			}
			right->left = node;
			node->parent = right;
			right->size = node->size;
			node->size = node->left->size + node->right->size + 1;
			right->sum = node->sum;
			node->sum = node->left->sum + node->right->sum + node->data;
		}

		void RotateRight(Node *node)
		{
			Node *left = node->left;
			Propagate(left);
			Propagate(node->right);
			Propagate(left->right);
			node->left = left->right;
			if (left->right != null)
			{
				left->right->parent = node;
			}
			left->parent = node->parent;
			if (node == root)
			{
				root = left;
			}
			else if (node == node->parent->right)
			{
				node->parent->right = left;
			}
			else
			{
				node->parent->left = left;
			}
			left->right = node;
			node->parent = left;
			left->size = node->size;
			node->size = node->left->size + node->right->size + 1;
			left->sum = node->sum;
			node->sum = node->left->sum + node->right->sum + node->data;
		}

		void FixInsert(Node *node)
		{
			Node *parent = node->parent, *grandparent = parent->parent, *uncle;
			while (parent->color == RED)
			{
				if (parent == grandparent->right)
				{
					uncle = grandparent->left;
					if (uncle->color == RED)
					{
						uncle->color = BLACK;
						parent->color = BLACK;
						grandparent->color = RED;
						node = grandparent;
					}
					else
					{
						if (node == parent->left)
						{
							node = parent;
							RotateRight(node);
						}
						node->parent->color = BLACK;
						node->parent->parent->color = RED;
						RotateLeft(node->parent->parent);
					}
				}
				else
				{
					uncle = grandparent->right;
					if (uncle->color == RED)
					{
						uncle->color = BLACK;
						parent->color = BLACK;
						grandparent->color = RED;
						node = grandparent;
					}
					else
					{
						if (node == parent->right)
						{
							node = parent;
							RotateLeft(node);
						}
						node->parent->color = BLACK;
						node->parent->parent->color = RED;
						RotateRight(node->parent->parent);
					}
				}
				if (node == root)
				{
					break;
				}
				parent = node->parent;
				grandparent = parent->parent;
			}
			root->color = BLACK;
		}

		void Insert(size_t pos, T data)
		{
			Node *node = new Node{data, data, 0, 1, RED, null, null, NULL}, *parent;
			if (root == null)
			{
				root = node;
				node->color = BLACK;
				return;
			}
			else
			{
				if (pos == root->size + 1)
				{
					parent = root;
					Propagate(parent);
					while (parent->right != null)
					{
						parent->size++;
						parent->sum += data;
						parent = parent->right;
						Propagate(parent);
					}
					parent->size++;
					parent->sum += data;
					parent->right = node;
					node->parent = parent;
				}
				else
				{
					parent = root;
					size_t idx = parent->left->size + 1;
					Propagate(parent);
					while (idx != pos)
					{
						parent->size++;
						parent->sum += data;
						if (pos < idx)
						{
							parent = parent->left;
						}
						else
						{
							pos -= idx;
							parent = parent->right;
						}
						idx = parent->left->size + 1;
						Propagate(parent);
					}
					parent->size++;
					parent->sum += data;
					if (parent->left == null)
					{
						parent->left = node;
						node->parent = parent;
					}
					else
					{
						parent = parent->left;
						Propagate(parent);
						while (parent->right != null)
						{
							parent->size++;
							parent->sum += data;
							parent = parent->right;
							Propagate(parent);
						}
						parent->size++;
						parent->sum += data;
						parent->right = node;
						node->parent = parent;
					}
				}
			}
			if (parent == root)
			{
				return;
			}
			FixInsert(node);
		}

		Node *BSTReplace(Node *node)
		{
			node->size--;
			node->sum -= node->data;
			if (node->left != null)
			{
				if (node->right != null)
				{
					node = node->right;
					Propagate(node);
					T data = Minimum(node)->data;
					while (node->left != null)
					{
						node->size--;
						node->sum -= data;
						node = node->left;
					}
					return node;
				}
				Propagate(node->left);
				return node->left;
			}
			if (node->right != null)
			{
				Propagate(node->right);
				return node->right;
			}
			return null;
		}

		Node *Sibling(Node *node)
		{
			if (node == root)
			{
				return null;
			}
			if (node == node->parent->left)
			{
				return node->parent->right;
			}
			return node->parent->left;
		}

		void FixDoubleBlack(Node *node)
		{
			while (node != root)
			{
				Node *parent = node->parent, *sibling = Sibling(node);
				if (sibling == null)
				{
					node = parent;
				}
				else
				{
					if (sibling->color == RED)
					{
						parent->color = RED;
						sibling->color = BLACK;
						if (sibling == parent->left)
						{
							RotateRight(parent);
						}
						else
						{
							RotateLeft(parent);
						}
					}
					else
					{
						if (sibling->left->color == RED || sibling->right->color == RED)
						{
							if (sibling->left->color == RED)
							{
								if (sibling == parent->left)
								{
									sibling->left->color = BLACK;
									sibling->color = parent->color;
									RotateRight(parent);
								}
								else
								{
									sibling->left->color = parent->color;
									Propagate(sibling);
									RotateRight(sibling);
									RotateLeft(parent);
								}
							}
							else
							{
								if (sibling == parent->left)
								{
									sibling->right->color = parent->color;
									Propagate(sibling);
									RotateLeft(sibling);
									RotateRight(parent);
								}
								else
								{
									sibling->right->color = BLACK;
									sibling->color = parent->color;
									RotateLeft(parent);
								}
							}
							parent->color = BLACK;
							return;
						}
						else
						{
							sibling->color = RED;
							if (parent->color == BLACK)
							{
								node = parent;
							}
							else
							{
								parent->color = BLACK;
								return;
							}
						}
					}
				}
			}
		}

		void Delete(Node *node)
		{
			Node *substitute = BSTReplace(node), *parent = node->parent, *sibling = Sibling(node);
			bool doubleblack = substitute->color == BLACK && node->color == BLACK;
			if (substitute == null)
			{
				if (node == root)
				{
					root = null;
				}
				else
				{
					if (doubleblack)
					{
						FixDoubleBlack(node);
					}
					else if (sibling != null)
					{
						sibling->color = RED;
					}
					if (node == parent->left)
					{
						parent->left = null;
					}
					else
					{
						parent->right = null;
					}
				}
				delete node;
			}
			else if (node->left == null || node->right == null)
			{
				if (node == root)
				{
					node->data = substitute->data;
					node->left = node->right = null;
					delete substitute;
				}
				else
				{
					if (node == parent->left)
					{
						parent->left = substitute;
					}
					else
					{
						parent->right = substitute;
					}
					delete node;
					substitute->parent = parent;
					if (doubleblack)
					{
						FixDoubleBlack(substitute);
					}
					else
					{
						substitute->color = BLACK;
					}
				}
			}
			else
			{
				node->data = substitute->data;
				Delete(substitute);
			}
		}

		void Delete(size_t pos)
		{
			Node *node = root;
			size_t idx = node->left->size + 1;
			T data = Search(pos)->data;
			while (idx != pos)
			{
				node->size--;
				node->sum -= data;
				if (pos < idx)
				{
					node = node->left;
				}
				else
				{
					pos -= idx;
					node = node->right;
				}
				idx = node->left->size + 1;
			}
			Delete(node);
		}

		Node *Minimum(Node *node)
		{
			while (node->left != null)
			{
				node = node->left;
				Propagate(node);
			}
			return node;
		}

		Node *Maximum(Node *node)
		{
			while (node->right != null)
			{
				node = node->right;
				Propagate(node);
			}
			return node;
		}

		Node *Successor(Node *node)
		{
			if (node->right != null)
			{
				Propagate(node->right);
				return Minimum(node->right);
			}
			Node *parent = node->parent;
			while (node == parent->right)
			{
				node = parent;
				parent = parent->parent;
			}
			return parent;
		}

		Node *Predecessor(Node *node)
		{
			if (node->left != null)
			{
				Propagate(node->left);
				return Maximum(node->left);
			}
			Node *parent = node->parent;
			while (node == parent->left)
			{
				node = parent;
				parent = parent->parent;
			}
			return parent;
		}

		void Clear(Node *node)
		{
			if (node == null)
			{
				return;
			}
			Clear(node->left);
			Clear(node->right);
			delete node;
		}

		void Propagate(Node *node)
		{
			if (node->lazy)
			{
				node->data += node->lazy;
				node->sum += node->lazy * node->size;
				if (node->left != null)
				{
					node->left->lazy += node->lazy;
				}
				if (node->right != null)
				{
					node->right->lazy += node->lazy;
				}
				node->lazy = 0;
			}
		}

		T Query(Node *node, size_t ss, size_t se, size_t qs, size_t qe)
		{
			Propagate(node);
			if (qs == ss && qe == se)
			{
				return node->sum;
			}
			size_t idx = node->left->size + 1;
			if (qs <= idx && qe >= idx)
			{
				return Query(node->left, 1, node->left->size, qs, idx - 1) + node->data + Query(node->right, 1, node->right->size, 1, qe - idx);
			}
			if (qe < idx)
			{
				return Query(node->left, 1, node->left->size, qs, qe);
			}
			return Query(node->right, 1, node->right->size, qs - idx, qe - idx);
		}

		void Update(Node *node, size_t ss, size_t se, size_t us, size_t ue, T diff)
		{
			if (us == ss && ue == se)
			{
				node->lazy += diff;
				return;
			}
			node->sum += diff * (ue - us + 1);
			size_t idx = node->left->size + 1;
			if (us <= idx && ue >= idx)
			{
				node->data += diff;
				if (us < idx)
				{
					Update(node->left, 1, node->left->size, us, idx - 1, diff);
				}
				if (ue > idx)
				{
					Update(node->right, 1, node->right->size, 1, ue - idx, diff);
				}
			}
			else if (ue < idx)
			{
				Update(node->left, 1, node->left->size, us, ue, diff);
			}
			else
			{
				Update(node->right, 1, node->right->size, us - idx, ue - idx, diff);
			}
		}

		size_t Index(Node *node)
		{
			size_t pos = node->left->size + 1;
			while (node != root)
			{
				Node *parent = node->parent;
				if (node == parent->right)
				{
					pos += parent->left->size + 1;
				}
				node = parent;
			}
			return pos;
		}

	public:
		dynamic_segment_tree()
		{
			null = new Node{0, 0, 0, 0, BLACK, NULL, NULL, NULL};
			root = null;
		}

		void insert(size_t pos, T data)
		{
			Insert(pos, data);
		}

		void erase(size_t pos)
		{
			Delete(pos);
		}

		void erase(Node *node)
		{
			Delete(Index(node));
		}

		Node *find(size_t pos)
		{
			return Search(pos);
		}

		T &at(size_t pos)
		{
			return Search(pos)->data;
		}

		T &operator[](size_t pos)
		{
			return Search(pos)->data;
		}

		T sum(size_t left, size_t right)
		{
			return Query(root, 1, root->size, left, right);
		}

		void update(size_t left, size_t right, T diff)
		{
			Update(root, 1, root->size, left, right, diff);
		}

		Node *next(Node *node)
		{
			return Successor(node);
		}

		Node *prev(Node *node)
		{
			return Predecessor(node);
		}

		size_t size()
		{
			return root->size;
		}

		size_t index(Node *node)
		{
			return Index(node);
		}

		bool empty()
		{
			return root == null;
		}

		void clear()
		{
			Clear(root);
			root = null;
		}
	};
} // namespace __rb_tree

#endif