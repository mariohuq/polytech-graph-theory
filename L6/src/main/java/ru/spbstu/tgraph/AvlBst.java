package ru.spbstu.tgraph;

import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

/**
 * AVL implementation of binary search tree.
 */
public class AvlBst<Key extends Comparable<Key>, Value>
        implements Bst<Key, Value> {

    private Node root;
    private int size;

    @Nullable
    private Value deleted;

    @Override
    @Nullable
    public Value get(@NotNull Key key) {
        return get(root, key);
    }

    @Override
    public void put(@NotNull Key key, @NotNull Value value) {
        root = put(root, key, value);
    }

    @Override
    @Nullable
    public Value remove(@NotNull Key key) {
        deleted = null;
        root = remove(root, key);
        return deleted;
    }

    @Override
    @Nullable
    public Key min() {
        if (root == null) {
            return null;
        } else {
            final Node min = root.min();
            assert min != null;
            return min.key;
        }
    }

    @Override
    @Nullable
    public Value minValue() {
        if (root == null) {
            return null;
        } else {
            final Node min = root.min();
            assert min != null;
            return min.value;
        }
    }

    @Override
    @Nullable
    public Key max() {
        if (root == null) {
            return null;
        } else {
            final Node max = root.max();
            assert max != null;
            return max.key;
        }
    }

    @Override
    @Nullable
    public Value maxValue() {
        if (root == null) {
            return null;
        } else {
            final Node max = root.max();
            assert max != null;
            return max.value;
        }
    }

    @Override
    @Nullable
    public Key floor(@NotNull Key key) {
        final Node floor = floor(root, key);
        return floor == null ? null : floor.key;
    }

    @Override
    @Nullable
    public Key ceil(@NotNull Key key) {
        final Node ceil = ceil(root, key);
        return ceil == null ? null : ceil.key;
    }

    @Override
    public int size() {
        return size;
    }

    // Высота дерева
    public int height() {
        return height(root);
    }

    private int height(Node x) {
        return x == null ? 0 : x.height;
    }

    @Nullable
    private Value get(@Nullable Node node, @NotNull Key key) {
        if (node == null) {
            return null;
        }
        final int compare = key.compareTo(node.key);
        if (compare < 0) {
            return get(node.left, key);
        }
        if (compare > 0) {
            return get(node.right, key);
        }
        return node.value;
    }

    @Nullable
    private Node floor(@Nullable Node node, @NotNull Key key) {
        if (node == null) {
            return null;
        }
        final int compare = key.compareTo(node.key);
        if (compare < 0) {
            return floor(node.left, key);
        }
        if (compare > 0) {
            final Node floor = floor(node.right, key);
            return floor == null ? node : floor;
        }
        return node;
    }

    @Nullable
    private Node ceil(@Nullable Node node, @NotNull Key key) {
        if (node == null) {
            return null;
        }
        final int compare = key.compareTo(node.key);
        if (compare < 0) {
            final Node ceil = ceil(node.left, key);
            return ceil == null ? node : ceil;
        }
        if (compare > 0) {
            return ceil(node.right, key);
        }
        return node;
    }

    @NotNull
    private Node put(@Nullable Node node, @NotNull Key key, Value value) {
        if (node == null) {
            size++;
            return new Node(key, value);
        }
        final int compare = key.compareTo(node.key);
        if (compare < 0) {
            node.left = put(node.left, key, value);
        } else if (compare > 0) {
            node.right = put(node.right, key, value);
        } else {
            node.value = value;
        }
        node.fixHeight();
        return node.balance();
    }

    @Nullable
    private Node remove(@Nullable Node node, Key key) {
        if (node == null) {
            return null;
        }
        final int compare = key.compareTo(node.key);
        if (compare == 0) {
            size--;
            deleted = node.value;
            return node.innerRemove();
        }
        if (compare < 0) {
            node.left = remove(node.left, key);
        } else {
            node.right = remove(node.right, key);
        }
        return node;
    }

    private class Node {
        @NotNull Key key;
        @Nullable Value value;
        @Nullable Node left;
        @Nullable Node right;
        int height = 1;

        Node(@NotNull Key key, @Nullable Value value) {
            this.key = key;
            this.value = value;
        }

        int factor() {
            return height(left) - height(right);
        }

        @NotNull
        Node balance() {
            switch (factor()) {
                case 2:
                    assert left != null;
                    if (left.factor() < 0) {
                        left = left.rotateLeft();
                    }
                    return rotateRight();
                case -2:
                    assert right != null;
                    if (right.factor() > 0) {
                        right = right.rotateRight();
                    }
                    return rotateLeft();
                default:
                    return this;
            }
        }

        @NotNull
        Node rotateRight() {
            final Node oldLeft = left;
            assert oldLeft != null;
            left = oldLeft.right;
            oldLeft.right = this;
            fixHeight();
            oldLeft.fixHeight();
            return oldLeft;
        }

        @NotNull
        Node rotateLeft() {
            final Node oldRight = right;
            assert oldRight != null;
            right = oldRight.left;
            oldRight.left = this;
            fixHeight();
            oldRight.fixHeight();
            return oldRight;
        }

        void fixHeight() {
            height = 1 + Math.max(height(left), height(right));
        }

        @Nullable
        private Node min() {
            return left == null ? this : left.min();
        }

        @Nullable
        private Node max() {
            return right == null ? this : right.max();
        }

        @Nullable
        Node removeMin() {
            if (left == null) {
                return right;
            }
            left = left.removeMin();
            return this;
        }

        @Nullable
        Node innerRemove() {
            if (right == null) {
                return left;
            }
            if (left == null) {
                return right;
            }
            final Node x = right.min();
            assert x != null;
            x.right = right.removeMin();
            x.left = left;
            return x;
        }
    }
}
