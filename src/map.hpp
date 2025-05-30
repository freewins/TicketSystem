/**
 * implement a container like std::map
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <functional>
#include <cstddef>
#include <stdexcept>

#include "pair.hpp"
#include "exceptions.hpp"

namespace sjtu {
  template<
    class Key,
    class T,
    class Compare = std::less<Key> >
  class map {
  private:
    enum class Color { RED, BLACK };

    struct node {
      pair<const Key, T> *data;
      node *left_;
      node *right_;
      node *father_;
      Color color_ = Color::RED;

      explicit node() {
        data = nullptr;
        left_ = right_ = father_ = nullptr;
        color_ = Color::RED;
      }

      explicit node(const node *ohter) {
        this->data = new pair<const Key, T>(ohter->data->first, ohter->data->second);
        this->color_ = ohter->color_;
        this->left_ = this->right_ = this->father_ = nullptr;
      }

      node(const Key &_key_, const T &_value_) {
        data = new pair<const Key, T>(_key_, _value_);
        left_ = right_ = father_ = nullptr;
        color_ = Color::RED;
      }

      ~node() {
        delete data;
      }
    };

    Color getColor(node *node_) {
      if (node_ != nullptr)
        return node_->color_;
      else
        return Color::BLACK;
    }

    static node *getSibling(node *cur) {
      if (cur->father_ != nullptr) {
        if (cur == cur->father_->left_) {
          return cur->father_->right_;
        } else {
          return cur->father_->left_;
        }
      }
      return nullptr;
    }

    static node *getUncle(node *cur) {
      if (cur->father_ != nullptr && cur->father_->father_ != nullptr) {
        return getSibling(cur->father_);
      } else return nullptr;
    }

    static node *getGrandFather(node *cur) {
      return cur->father_->father_;
    }

    node *root_;
    node *end_node;
    size_t size_;
    Compare compare_ = Compare();

    /**
     * To check if the double black node is to check.
     * @param cur node* if cancel cur will change nullptr
     */
    void CancelDoubleBlack(node * &cur) {
      if (cur == root_ || getColor(cur) == Color::RED) {
        cur->color_ = Color::BLACK;
        cur = nullptr;
      } else {
        return;
      }
    }

    void SwitchFaterSon(node *father, node *son) {
      //Son must be father right
      node *f_left = father->left_;
      father->left_ = son->left_;
      if (father->left_ != nullptr)
        father->left_->father_ = father;
      father->right_ = son->right_;
      if (father->right_ != nullptr)
        father->right_->father_ = father;
      Color tmp = son->color_;
      son->color_ = father->color_;
      father->color_ = tmp;
      son->father_ = father->father_;
      if (son->father_ != nullptr) {
        if (son->father_->left_ == father) {
          son->father_->left_ = son;
        } else {
          son->father_->right_ = son;
        }
      } else {
        root_ = son;
      }
      father->father_ = son;
      son->right_ = father;
      son->left_ = f_left;
      if (son->left_ != nullptr) {
        son->left_->father_ = son;
      }
    }

    /**
     * Swtich the node relation
     * @param cur_a next_node
     * @param cur_b delete node
     */
    void SwitchNode(node *cur_a, node *cur_b) {
      node *tmp = new node();
      tmp->left_ = cur_a->left_;
      tmp->right_ = cur_a->right_;
      tmp->father_ = cur_a->father_;
      tmp->color_ = cur_a->color_;

      cur_a->father_ = cur_b->father_;
      if (cur_b->father_ != nullptr) {
        if (cur_b == cur_b->father_->left_) {
          cur_b->father_->left_ = cur_a;
        } else {
          cur_b->father_->right_ = cur_a;
        }
      } else {
        //If father is nullptr ,is root
        root_ = cur_a;
      }
      cur_a->left_ = cur_b->left_;
      if (cur_a->left_ != nullptr) {
        cur_a->left_->father_ = cur_a;
      }
      cur_a->right_ = cur_b->right_;
      if (cur_a->right_ != nullptr) {
        cur_a->right_->father_ = cur_a;
      }
      cur_a->color_ = cur_b->color_;

      cur_b->father_ = tmp->father_;
      if (cur_b->father_ != nullptr) {
        if (cur_a == cur_b->father_->left_) {
          cur_b->father_->left_ = cur_b;
        } else {
          cur_b->father_->right_ = cur_b;
        }
      } else {
        root_ = cur_b;
      }
      cur_b->right_ = tmp->right_;
      if (cur_b->right_ != nullptr) {
        cur_b->right_->father_ = cur_b;
      }
      cur_b->left_ = tmp->left_;
      if (cur_b->left_ != nullptr) {
        cur_b->left_->father_ = cur_b;
      }
      cur_b->color_ = tmp->color_;
      delete tmp;
    }

    void ChangeColor(node *cur) {
      if (cur == nullptr)
        return;
      if (cur->color_ == Color::RED) {
        cur->color_ = Color::BLACK;
      } else {
        cur->color_ = Color::RED;
      }
    }

    void RotateRight(node *node_) {
      node *child = node_->left_;
      node *grand_child = child->right_;

      node_->left_ = grand_child;
      if (grand_child != nullptr) {
        grand_child->father_ = node_;
      }

      child->right_ = node_;
      child->father_ = node_->father_;
      node_->father_ = child;

      if (child->father_ != nullptr) {
        if (child->father_->left_ == node_) {
          child->father_->left_ = child;
        } else {
          child->father_->right_ = child;
        }
      } else {
        root_ = child;
      }
    }

    void RotateLeft(node *node_) {
      node *child = node_->right_;
      node *grand_child = child->left_;
      node_->right_ = grand_child;

      if (grand_child != nullptr) {
        grand_child->father_ = node_;
      }

      child->left_ = node_;
      child->father_ = node_->father_;
      node_->father_ = child;

      if (child->father_ != nullptr) {
        if (child->father_->left_ == node_) {
          child->father_->left_ = child;
        } else {
          child->father_->right_ = child;
        }
      } else {
        root_ = child;
      }
    }

    void Remove(node *cur) {
      if (cur->left_ != nullptr && cur->right_ != nullptr) {
        node *p = cur->right_;
        bool change = false;
        while (p->left_ != nullptr) {
          p = p->left_;
          change = true;
        }
        //pay attention
        //if delete and change directly,the iterator to this point will be invalid.
        //change relation,not the value
        if (change)SwitchNode(p, cur);
        else SwitchFaterSon(cur, p);
        Remove(cur);
      } else if (cur->left_ == nullptr && cur->right_ != nullptr) {
        node *child = cur->right_;
        child->father_ = cur->father_;
        //Abstract to a function
        if (cur->father_) {
          if (cur == cur->father_->right_) {
            cur->father_->right_ = child;
          } else {
            cur->father_->left_ = child;
          }
        } else {
          root_ = child;
        }
        delete cur;
        ChangeColor(child);
      } else if (cur->left_ != nullptr && cur->right_ == nullptr) {
        node *child = cur->left_;
        child->father_ = cur->father_;
        if (cur->father_) {
          if (cur == cur->father_->right_) {
            cur->father_->right_ = child;
          } else {
            cur->father_->left_ = child;
          }
        } else {
          root_ = child;
        }
        delete cur;
        ChangeColor(child);
      } else {
        if (cur == root_) {
          root_ = nullptr;
          delete cur;
          return;
        }
        if (cur->color_ == Color::RED) {
          if (cur == cur->father_->right_) {
            cur->father_->right_ = nullptr;
          } else {
            cur->father_->left_ = nullptr;
          }
          delete cur;
        } else {
          node *double_black = cur; //double_black node
          while (double_black != nullptr) {
            node *sibling = getSibling(double_black);
            if (getColor(sibling) == Color::RED) {
              //Sibling is red
              ChangeColor(sibling);
              ChangeColor(double_black->father_);
              if (double_black == double_black->father_->right_) {
                RotateRight(double_black->father_);
              } else {
                RotateLeft(double_black->father_);
              }
              continue;
              //double_black = double_black->father_;
            } else {
              //sibling is black and sons of it is all black
              if (sibling == nullptr ||
                  ((getColor(sibling->left_) == Color::BLACK && getColor(sibling->right_) == Color::BLACK))) {
                ChangeColor(sibling);
                double_black = double_black->father_;
              } else {
                //sibling has at least a red son
                if (sibling == sibling->father_->left_) {
                  if (getColor(sibling->left_) == Color::RED) {
                    //LL Type
                    sibling->left_->color_ = sibling->color_;
                    sibling->color_ = sibling->father_->color_;
                    sibling->father_->color_ = Color::BLACK;
                    RotateRight(sibling->father_);
                    double_black = nullptr;
                    break;
                  } else {
                    //LR Type
                    /**
                     *   G                G
                     *  / \    ->        / \
                     *  Sib             Son
                     *   \              /
                     *    Son          Sib
                     */
                    sibling->right_->color_ = sibling->father_->color_;
                    sibling->father_->color_ = Color::BLACK;
                    RotateLeft(sibling);
                    RotateRight(sibling->father_->father_);
                    double_black = nullptr;
                    break;
                  }
                } else {
                  if (getColor(sibling->right_) == Color::RED) {
                    //RR Type
                    sibling->right_->color_ = sibling->color_;
                    sibling->color_ = sibling->father_->color_;
                    sibling->father_->color_ = Color::BLACK;
                    RotateLeft(sibling->father_);
                    double_black = nullptr;
                    break;
                  } else {
                    //RL
                    /**
                     *    G           G
                     *   / \         / \
                     *     Sib  ->      Son
                     *      /            \
                     *     Son            Sib
                     */
                    sibling->left_->color_ = sibling->father_->color_;
                    sibling->father_->color_ = Color::BLACK;
                    RotateRight(sibling);
                    RotateLeft(sibling->father_->father_);
                    double_black = nullptr;
                    break;
                  }
                }
              }
            }
            CancelDoubleBlack(double_black);
          }
          if (cur == cur->father_->right_) {
            cur->father_->right_ = nullptr;
          } else {
            cur->father_->left_ = nullptr;
          }
          delete cur;
        }
      }
    }

    void Delete(node *cur) {
      if (cur == nullptr)
        return;
      Delete(cur->left_);
      Delete(cur->right_);
      delete cur;
    }

    node *helpConstruct(node *node_, node *father_) {
      if (node_ == nullptr)
        return nullptr;
      node *cur = new node(node_);
      cur->father_ = father_;
      cur->right_ = helpConstruct(node_->right_, cur);
      cur->left_ = helpConstruct(node_->left_, cur);
      return cur;
    }

    node *search(const Key &key) const {
      if (root_ == nullptr) {
        return nullptr;
      } else {
        node *cur = root_;
        while (cur != nullptr) {
          if (compare_(key, cur->data->first)) {
            cur = cur->left_;
          } else if (compare_(cur->data->first, key)) {
            cur = cur->right_;
          } else {
            return cur;
          }
        }
      }
      return nullptr;
    }

  public:
    /**
     * the internal type of data.
     * it should have a default constructor, a copy constructor.
     * You can use sjtu::map as value_type by typedef.
     */
    typedef pair<const Key, T> value_type;
    /**
     * see BidirectionalIterator at CppReference for help.
     *
     * if there is anything wrong throw invalid_iterator.
     *     like it = map.begin(); --it;
     *       or it = map.end(); ++end();
     */
    class const_iterator;

    friend class const_iterator;
    friend class iterator;

    class iterator {
    private:
    public:
      const map *map_ptr;
      node *ptr;

      iterator() {
        ptr = nullptr;
        map_ptr = nullptr;
      }

      iterator(node *ptr_, const map *map_ptr_) {
        ptr = ptr_;
        map_ptr = map_ptr_;
      }

      iterator(const iterator &other) {
        this->map_ptr = other.map_ptr;
        this->ptr = other.ptr;
      }

      iterator operator++(int) {
        iterator tmp(ptr, map_ptr);
        ++(*this);
        return tmp;
      }


      iterator &operator++() {
        if (ptr == nullptr || ptr == map_ptr->end_node) {
          throw sjtu::invalid_iterator();
        }
        if (ptr->right_ != nullptr) {
          ptr = ptr->right_;
          while (ptr->left_ != nullptr) {
            ptr = ptr->left_;
          }
        } else {
          node *parent = ptr->father_;
          while (parent != nullptr && ptr == parent->right_) {
            ptr = parent;
            parent = parent->father_;
          }
          if (parent == nullptr) {
            ptr = map_ptr->end_node;
          } else {
            ptr = parent;
          }
        }
        return *this;
      }


      iterator operator--(int) {
        iterator tmp(ptr, map_ptr);
        --(*this);
        return tmp;
      }


      iterator &operator--() {
        if (ptr == nullptr || map_ptr->root_ == nullptr) {
          throw sjtu::invalid_iterator();
        }
        if (ptr == map_ptr->end_node) {
          ptr = map_ptr->root_;
          if (ptr == nullptr) {
            throw sjtu::invalid_iterator();
          }
          while (ptr->right_ != nullptr) {
            ptr = ptr->right_;
          }
        } else if (ptr->left_ != nullptr) {
          ptr = ptr->left_;
          while (ptr->right_ != nullptr) {
            ptr = ptr->right_;
          }
        } else {
          node *parent = ptr->father_;
          while (parent != nullptr && ptr == parent->left_) {
            ptr = parent;
            parent = parent->father_;
          }
          if (parent == nullptr) {
            throw sjtu::invalid_iterator();
          } else {
            ptr = parent;
          }
        }
        return *this;
      }

      /**
       * a operator to check whether two iterators are same (pointing to the same memory).
       */
      value_type &operator*() const {
        return *(ptr->data);
      }

      bool operator==(const iterator &rhs) const {
        return ptr == rhs.ptr;
      }

      bool operator==(const const_iterator &rhs) const {
        return ptr == rhs.ptr;
      }

      /**
       * some other operator for iterator.
       */
      bool operator!=(const iterator &rhs) const {
        return ptr != rhs.ptr;
      }

      bool operator!=(const const_iterator &rhs) const {
        return ptr != rhs.ptr;
      }

      /**
       * for the support of it->first.
       * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
       */
      value_type *operator->() const noexcept {
        if (ptr != nullptr) {
          return ptr->data;
        } else return nullptr;
      }
    };

    class const_iterator {
      // it should has similar member method as iterator.
      //  and it should be able to construct from an iterator.
    private:
      // data members.


    public:
      const map *map_ptr;
      node *ptr;

      const_iterator() {
        ptr = nullptr;
      }

      const_iterator(node *ptr_, const map *map_ptr_) {
        ptr = ptr_;
        map_ptr = map_ptr_;
      }

      const_iterator(const const_iterator &other) {
        this->map_ptr = other.map_ptr;
        this->ptr = other.ptr;
      }

      const_iterator(const iterator &other) {
        this->map_ptr = other.map_ptr;
        this->ptr = other.ptr;
      }

      // And other methods in iterator.
      // And other methods in iterator.
      // And other methods in iterator.
      const_iterator operator++(int) {
        const_iterator tmp(ptr, map_ptr);
        ++(*this);
        return tmp;
      }


      const_iterator &operator++() {
        if (ptr == nullptr || ptr == map_ptr->end_node) {
          throw sjtu::invalid_iterator();
        }
        if (ptr->right_ != nullptr) {
          ptr = ptr->right_;
          while (ptr->left_ != nullptr) {
            ptr = ptr->left_;
          }
        } else {
          node *parent = ptr->father_;
          while (parent != nullptr && ptr == parent->right_) {
            ptr = parent;
            parent = parent->father_;
          }
          if (parent == nullptr) {
            ptr = map_ptr->end_node;
          } else {
            ptr = parent;
          }
        }
        return *this;
      }

      /**
       *
       */
      const_iterator operator--(int) {
        const_iterator tmp(ptr, map_ptr);
        --(*this);
        return tmp;
      }

      /**
       *
       */
      const_iterator &operator--() {
        if (ptr == nullptr || map_ptr->root_ == nullptr) {
          throw sjtu::invalid_iterator();
        }
        if (ptr == map_ptr->end_node) {
          ptr = map_ptr->root_;
          if (ptr == nullptr) {
            throw sjtu::invalid_iterator();
          }
          while (ptr->right_ != nullptr) {
            ptr = ptr->right_;
          }
        } else if (ptr->left_ != nullptr) {
          ptr = ptr->left_;
          while (ptr->right_ != nullptr) {
            ptr = ptr->right_;
          }
        } else {
          node *parent = ptr->father_;
          while (parent != nullptr && ptr == parent->left_) {
            ptr = parent;
            parent = parent->father_;
          }
          if (parent == nullptr) {
            throw sjtu::invalid_iterator();
          } else {
            ptr = parent;
          }
        }
        return *this;
      }

      /**
       * a operator to check whether two iterators are same (pointing to the same memory).
       */
      const value_type &operator*() const {
        return *(ptr->data);
      }

      bool operator==(const iterator &rhs) const {
        return ptr == rhs.ptr;
      }

      bool operator==(const const_iterator &rhs) const {
        return ptr == rhs.ptr;
      }

      /**
       * some other operator for iterator.
       */
      bool operator!=(const iterator &rhs) const {
        return ptr != rhs.ptr;
      }

      bool operator!=(const const_iterator &rhs) const {
        return ptr != rhs.ptr;
      }

      /**
       * for the support of it->first.
       * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
       */
      const value_type *operator->() const noexcept {
        if (ptr != nullptr) {
          return ptr->data;
        } else return nullptr;
      }
    };

    /**
     *
     */

    map() {
      end_node = new node();
      root_ = nullptr;
      size_ = 0;
    }

    map(const map &other) {
      end_node = new node();
      this->size_ = other.size_;
      root_ = helpConstruct(other.root_, nullptr);
    }

    /**
     *
     */
    map &operator=(const map &other) {
      if (this->root_ == other.root_) {
        return *this;
      }
      this->clear();
      this->size_ = other.size_;
      root_ = helpConstruct(other.root_, nullptr);
      return *this;
    }

    /**
     *
     */
    ~map() {
      Delete(root_);
      delete end_node;
    }

    /**
     *
     * access specified element with bounds checking
     * Returns a reference to the mapped value of the element with key equivalent to key.
     * If no such element exists, an exception of type `index_out_of_bound'
     */
    T &at(const Key &key) {
      node *cur = root_;
      while (cur != nullptr) {
        if (compare_(key, cur->data->first)) {
          cur = cur->left_;
        } else if (compare_(cur->data->first, key)) {
          cur = cur->right_;
        } else {
          return cur->data->second;
        }
      }
      throw sjtu::index_out_of_bound();
    }

    const T &at(const Key &key) const {
      node *cur = root_;
      while (cur != nullptr) {
        if (compare_(key, cur->data->first)) {
          cur = cur->left_;
        } else if (compare_(cur->data->first, key)) {
          cur = cur->right_;
        } else {
          return cur->data->second;
        }
      }
      throw sjtu::index_out_of_bound();
    }

    /**
     *
     * access specified element
     * Returns a reference to the value that is mapped to a key equivalent to key,
     *   performing an insertion if such key does not already exist.
     */
    T &operator[](const Key &key) {
      node *cur = search(key);
      if (cur == nullptr) {
        iterator new_node = this->insert(value_type(key, T())).first;
        return new_node.ptr->data->second;
      } else {
        return cur->data->second;
      }
    }

    /**
     * behave like at() throw index_out_of_bound if such key does not exist.
     */
    const T &operator[](const Key &key) const {
      return this->at(key);
    }

    /**
     * return a iterator to the beginning
     */
    iterator begin() {
      if (size_ == 0) {
        return iterator(end_node, this);
      }
      node *cur = root_;
      while (cur->left_ != nullptr) {
        cur = cur->left_;
      }
      return iterator(cur, this);
    }

    const_iterator cbegin() const {
      if (size_ == 0) {
        return const_iterator(end_node, this);
      }
      node *cur = root_;
      while (cur->left_ != nullptr) {
        cur = cur->left_;
      }
      return const_iterator(cur, this);
    }

    /**
     * return a iterator to the end
     * in fact, it returns past-the-end.
     */
    iterator end() {
      return iterator(end_node, this);
    }

    const_iterator cend() const {
      return const_iterator(end_node, this);
    }

    /**
     * checks whether the container is empty
     * return true if empty, otherwise false.
     */
    bool empty() const {
      return size_ == 0;
    }

    /**
     * returns the number of elements.
     */
    size_t size() const {
      return size_;
    }

    /**
     * clears the contents
     */
    void clear() {
      Delete(root_);
      root_ = nullptr;
      size_ = 0;
    }

    /**
     * insert an element.
     * return a pair, the first of the pair is
     *   the iterator to the new element (or the element that prevented the insertion),
     *   the second one is true if insert successfully, or false.
     */
    pair<iterator, bool> insert(const value_type &value) {
      if (root_ == nullptr) {
        root_ = new node(value.first, value.second);
        root_->color_ = Color::BLACK;
        root_->father_ = nullptr;
        size_++;
        return pair<iterator, bool>(iterator(root_, this), true);
      } else {
        node *parent = nullptr;
        node *cur = root_;
        while (cur != nullptr) {
          // If new data is bigger than cur, then insert pos is right
          if (compare_((cur->data)->first, value.first)) {
            parent = cur;
            cur = cur->right_;
          } else if (compare_(value.first, (cur->data)->first)) {
            parent = cur;
            cur = cur->left_;
          } else {
            //The value is existing
            return pair<iterator, bool>(iterator(cur, this), false);
          }
        }

        cur = new node(value.first, value.second);
        //TO return the correct p
        node *new_node = cur;

        cur->color_ = Color::RED;
        cur->father_ = parent;
        if (compare_(parent->data->first, value.first)) {
          parent->right_ = cur;
        } else {
          parent->left_ = cur;
        }

        while (parent != nullptr && parent->color_ == Color::RED) {
          if (getColor(getUncle(cur)) == Color::RED) {
            ChangeColor(parent);
            ChangeColor(getUncle(cur));
            ChangeColor(getGrandFather(cur));
            cur = getGrandFather(cur);
            parent = cur->father_;
          } else {
            /**
             *	   G
             *		/ \
             *		F
             *	/
             *	S
             */
            if (cur == parent->left_ && parent == parent->father_->left_) {
              ChangeColor(parent);
              ChangeColor(parent->father_);
              RotateRight(parent->father_);
            }
            /**
             *		 G
             *		/ \
             *		  F
             *	     \
             *	      S
             */
            else if (cur == parent->right_ && parent == parent->father_->right_) {
              ChangeColor(parent);
              ChangeColor(parent->father_);
              RotateLeft(parent->father_);
            }
            /**
             *       G          G
             *      / \        / \
             *         F  ->      S
             *        /            \
             *       S              F
             */
            else if (cur == parent->left_ && parent == parent->father_->right_) {
              ChangeColor(cur);
              ChangeColor(parent->father_);
              RotateRight(parent);
              RotateLeft(cur->father_);
            }
            /**
             *       G             G*
             *      / \           / \
             *     F      ->     S*
             *      \           /
             *       S         F
             */
            else {
              ChangeColor(cur);
              ChangeColor(parent->father_);
              RotateLeft(parent);
              RotateRight(cur->father_);
            }
            break;
          }
        }

        root_->color_ = Color::BLACK;
        size_++;
        return pair<iterator, bool>(iterator(new_node, this), true);
      }
    }

    /**
     * erase the element at pos.
     *
     * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
     */
    void erase(iterator pos) {
      if (root_ == nullptr || pos == this->end()) {
        throw sjtu::index_out_of_bound();
      }
      if (pos.map_ptr != this) {
        throw sjtu::invalid_iterator();
      }

      if (pos.ptr == nullptr) {
        throw sjtu::index_out_of_bound();
      } else {
        this->Remove(pos.ptr);
        size_--;
      }
    }

    /**
     * Returns the number of elements with key
     *   that compares equivalent to the specified argument,
     *   which is either 1 or 0
     *     since this container does not allow duplicates.
     * The default method of check the equivalence is !(a < b || b > a)
     */
    size_t count(const Key &key) const {
      node *resFind = this->search(key);
      if (resFind == nullptr) {
        return 0;
      } else {
        return 1;
      }
    }

    /**
     * Finds an element with key equivalent to key.
     * key value of the element to search for.
     * Iterator to an element with key equivalent to key.
     *   If no such element is found, past-the-end (see end()) iterator is returned.
     */
    iterator find(const Key &key) {
      node *resFind = search(key);
      if (resFind == nullptr) {
        return end();
      } else {
        return iterator(resFind, this);
      }
    }

    const_iterator find(const Key &key) const {
      node *resFind = search(key);
      if (resFind == nullptr) {
        return cend();
      } else {
        return const_iterator(resFind, this);
      }
    }
  };
}

#endif
