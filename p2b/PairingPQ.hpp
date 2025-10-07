// Project identifier: 43DE0E0C4C76BFAA6D8C2F5AEAE0518A9C251F4E

#ifndef PAIRINGPQ_H
#define PAIRINGPQ_H

#include <deque>
#include <utility>

#include "Eecs281PQ.hpp"

// A specialized version of the priority queue ADT implemented as a pairing
// heap.
template<typename TYPE, typename COMP_FUNCTOR = std::less<TYPE>>
class PairingPQ : public Eecs281PQ<TYPE, COMP_FUNCTOR> {
    // This is a way to refer to the base class object.
    using BaseClass = Eecs281PQ<TYPE, COMP_FUNCTOR>;

public:
    // Each node within the pairing heap
    class Node {
    public:
        // Description: Custom constructor that creates a node containing
        //              the given value.
        explicit Node(const TYPE &val)
            : elt { val } {}

        // Description: Allows access to the element at that Node's position.
        //              There are two versions, getElt() and a dereference
        //              operator, use whichever one seems more natural to you.
        // Runtime: O(1) - this has been provided for you.
        const TYPE &getElt() const { return elt; }
        const TYPE &operator*() const { return elt; }

        // The following line allows you to access any private data
        // members of this Node class from within the PairingPQ class.
        // (ie: myNode.elt is a legal statement in PairingPQ's add_node()
        // function).
        friend PairingPQ;

    private:
        TYPE elt;
        Node *child = nullptr;
        Node *sibling = nullptr;
        // TODO: Add and initialize one extra pointer (parent or previous) as desired.
        Node *parent = nullptr;
    };  // Node


    // Description: Construct an empty pairing heap with an optional
    //              comparison functor.
    // Runtime: O(1)
    explicit PairingPQ(COMP_FUNCTOR comp = COMP_FUNCTOR())
        : BaseClass { comp }, root {nullptr}, count {0} {
        
        
    }  // PairingPQ()


    // Description: Construct a pairing heap out of an iterator range with an
    //              optional comparison functor.
    // Runtime: O(n) where n is number of elements in range.
    template<typename InputIterator>
    PairingPQ(InputIterator start, InputIterator end, COMP_FUNCTOR comp = COMP_FUNCTOR())
        : BaseClass { comp },  root {nullptr}, count {0} {
        for (auto it = start; it != end; it++) {
            addNode(*it);
        }
    }  // PairingPQ()


    // Description: Copy constructor.
    // Runtime: O(n)
    PairingPQ(const PairingPQ &other)
        : BaseClass { other.compare }, root {nullptr}, count (0) {
        if (!other.root) return;

        std::deque<Node*> q;
        q.push_back(other.root);

        while(!q.empty()) {
            Node* cur = q.front();
            q.pop_front();

            addNode(cur->elt);
            
            if (cur->child) q.push_back(cur->child);
            if (cur->sibling) q.push_back(cur->sibling);

       

            
        }

    }  // PairingPQ()


    // Description: Copy assignment operator.
    // Runtime: O(n)
    PairingPQ &operator=(const PairingPQ &rhs) {
        // TODO: Implement this function.
        // HINT: Use the copy-swap method from the "Arrays and Containers"
        // lecture.
        if (this == &rhs) return *this;
        PairingPQ temp(rhs);
        std::swap(this->root, temp.root);
        std::swap(this->count, temp.count);
        std::swap(this->compare, temp.compare);

        return *this;


        
    }  // operator=()


    // Description: Destructor
    // Runtime: O(n)
    ~PairingPQ() {
        // TODO: Implement this function.
        if (!root) return;
        std::deque<Node*> stack;
        stack.push_back(root);
        while (!stack.empty()) {
            Node* node = stack.back();
            stack.pop_back();

            if (node->child) stack.push_back(node->child);
            if (node->sibling) stack.push_back(node->sibling);
            delete(node);
        }

        root = nullptr;

        count = 0;
    }  // ~PairingPQ()


    // Description: Move constructor and assignment operatos don't need any
    //              code, the members will be reused automatically.
    PairingPQ(PairingPQ &&) noexcept = default;
    PairingPQ &operator=(PairingPQ &&) noexcept = default;


    // Description: Assumes that all elements inside the pairing heap are out
    //              of order and 'rebuilds' the pairing heap by fixing the
    //              pairing heap invariant.  You CANNOT delete 'old' nodes
    //              and create new ones!
    // Runtime: O(n)
    virtual void updatePriorities() {
        // TODO: Implement this function.
        if (!root) return;

        std::deque<Node*> queue;
        queue.push_back(root);
        root = nullptr;
        
        while (!queue.empty()) {
            Node* cur = queue.front();
            queue.pop_front();

            // bfs
            if (cur->child) queue.push_back(cur->child);
            if (cur->sibling) queue.push_back(cur->sibling);

            // break cons
            cur->parent = nullptr;
            cur->sibling = nullptr;
            cur->child = nullptr;

            // meld
            root = meld(root, cur);

        }
    }  // updatePriorities()


    // Description: Add a new element to the pairing heap. This is already
    //              done. You should implement push functionality entirely
    //              in the addNode() function, and this function calls
    //              addNode().
    // Runtime: O(1)
    virtual void push(const TYPE &val) { addNode(val); }  // push()


    // Description: Remove the most extreme (defined by 'compare') element
    //              from the pairing heap.
    // Note: We will not run tests on your code that would require it to pop
    // an element when the pairing heap is empty. Though you are welcome to
    // if you are familiar with them, you do not need to use exceptions in
    // this project.
    // Runtime: Amortized O(log(n))
    virtual void pop() {
        // TODO: Implement this function.
        // Step1: break down all connections to root
        if (!root) return;

        Node* oldRoot = root;
        Node* child = root->child;

        std::deque<Node*> queue;

        while(child) {
            Node* next = child->sibling;
            child->parent = nullptr;
            child->sibling = nullptr;
            queue.push_back(child);
            child = next;
        }

        delete(oldRoot);
        count--;

        // Step2: meld all children
        while (queue.size() > 1) {
            Node* first = queue.front(); 
            queue.pop_front();
            Node* second = queue.front();
            queue.pop_front();
            
            queue.push_back(meld(first, second));
        }

        root = queue.empty() ? nullptr : queue.front();

        
    }  // pop()


    // Description: Return the most extreme (defined by 'compare') element of
    //              the pairing heap. This should be a reference for speed.
    //              It MUST be const because we cannot allow it to be
    //              modified, as that might make it no longer be the most
    //              extreme element.
    // Runtime: O(1)
    virtual const TYPE &top() const {
        // TODO: Implement this function
        return root->getElt();
    }  // top()

    // Description: Get the number of elements in the pairing heap.
    // Runtime: O(1)
    [[nodiscard]] virtual std::size_t size() const {
        // TODO: Implement this function
        return count;
    }  // size()

    // Description: Return true if the pairing heap is empty.
    // Runtime: O(1)
    [[nodiscard]] virtual bool empty() const {
        // TODO: Implement this function
        return root == nullptr;
    }  // empty()


    // Description: Updates the priority of an element already in the pairing
    //              heap by replacing the element refered to by the Node with
    //              new_value.  Must maintain pairing heap invariants.
    //
    // PRECONDITION: The new priority, given by 'new_value' must be more
    //              extreme (as defined by comp) than the old priority.
    //
    // Runtime: As discussed in reading material.
    void updateElt(Node *node, const TYPE &new_value) {
        // TODO: Implement this function
        node->elt = new_value;
        
        if (node == root) return;
        
        // break the node's cons
        Node* parent = node->parent;
        if (this->compare(parent->elt, node->elt)) {
            if (parent->child == node) {
                parent->child = node->sibling;
            } else {
                Node* cur = parent->child;
                while(cur->sibling != node) {
                    cur = cur->sibling;
                }
                cur->sibling = node->sibling;
            }
        }
        node->parent = nullptr;
        node->sibling = nullptr;

        // meld node and root
        root = meld(root, node);

        
    }  // updateElt()


    // Description: Add a new element to the pairing heap. Returns a Node*
    //              corresponding to the newly added element.
    // Runtime: O(1)
    // NOTE: Whenever you create a node, and thus return a Node *, you must
    //       be sure to never move or copy/delete that node in the future,
    //       until it is eliminated by the user calling pop(). Remember this
    //       when you implement updateElt() and updatePriorities().
    Node *addNode(const TYPE &val) {
        Node* nNode = new Node(val);
        root = meld(root, nNode);
        count++;
        return nNode;
    }  // addNode()


private:
    // TODO: Add any additional member variables or member functions you
    // require here.
    // TODO: We recommend creating a 'meld' function (see the Pairing Heap
    // papers).
    Node* root; // 
    std::size_t count; // The number of nodes


    // Time Com: O(1)
    Node* meld(Node* a, Node* b) {
        if (!a) return b;
        if (!b) return a;

        if (this->compare(a->elt, b->elt)) {
            // b is more extreme, so attach a as the first child of b
            a->sibling = b->child;
            b->child = a;
            a->parent = b;
            return b;
            
        } else {
            // a has higher priority so attach b as the first child of a
            b->sibling = a->child;
            a->child = b;
            b->parent = a;

            return a;
        }
    }

    


};


#endif  // PAIRINGPQ_H
