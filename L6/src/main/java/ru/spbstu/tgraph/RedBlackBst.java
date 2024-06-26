package ru.spbstu.tgraph;

import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

/**
 * LLRB implementation of binary search tree.
 */
public class RedBlackBst<Key extends Comparable<Key>, Value>
        implements Bst<Key, Value> {

    private static final boolean BLACK = false;
    private static final boolean RED = true;

    private Node root;
    private int size;

    @Nullable
    private Value deleted;

    @Nullable
    @Override
    public Value get(@NotNull Key key) {
        return get(root, key);
    }

    @Override
    public void put(@NotNull Key key, @NotNull Value value) {
        root = put(root, key, value);
        root.setBlack();
    }

    @Nullable
    @Override
    public Value remove(@NotNull Key key) {
        deleted = null;
        root = remove(root, key);
        return deleted;
    }

    @Nullable
    public Key min() {
        if (root == null) {
            return null;
        }
        return root.min().key;
    }

    @Nullable
    public Value minValue() {
        if (root == null) {
            return null;
        }
        return root.min().value;
    }

    @Nullable
    public Key max() {
        if (root == null) {
            return null;
        }
        return root.max().key;
    }

    @Nullable
    public Value maxValue() {
        if (root == null) {
            return null;
        }
        return root.max().value;
    }

    @Nullable
    public Key floor(@NotNull Key key) {
        final Node floor = floor(root, key);
        return floor == null ? null : floor.key;
    }

    @Nullable
    public Key ceil(@NotNull Key key) {
        final Node ceil = ceil(root, key);
        return ceil == null ? null : ceil.key;
    }

    @Override
    public int size() {
        return size;
    }

    // start snippet clear
    @Override
    public void clear() {
        root = null;
        size = 0;
    }
    // end snippet clear
    // start snippet get
    private Value get(@Nullable Node node, @NotNull Key key) {
        if (node == null) {
            return null;
        }
        int compare = key.compareTo(node.key);
        if (compare < 0) {
            return get(node.left, key);
        }
        if (compare > 0) {
            return get(node.right, key);
        }
        return node.value;
    }
    // end snippet get
    // start snippet put
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
        return fixUp(node);
    }
    // end snippet put
    private Node removeMin(Node node) {
        if (node.left == null) {
            return null;
        }
        if (!isRed(node.left) && !isRed(node.left.left)) {
            node = node.moveRedLeft();
        }
        node.left = removeMin(node.left);
        return fixUp(node);
    }
    // start snippet remove
    private Node remove(@Nullable Node node, @NotNull Key key) {
        if (node == null) {
            return null;
        }
        if (key.compareTo(node.key) < 0) {
            if (node.left == null) {
                return fixUp(node);
            }
            if (!isRed(node.left) && !isRed(node.left.left)) {
                node = node.moveRedLeft();
            }
            node.left = remove(node.left, key);
            return fixUp(node);
        }
        if (isRed(node.left)) {
            node = node.rotateRight();
            node.right = remove(node.right, key);
            return fixUp(node);
        }
        if (key.compareTo(node.key) == 0 && node.right == null) {
            deleted = node.value;
            size--;
            return null;
        }
        if (node.right != null && !isRed(node.right) && !isRed(node.right.left)) {
            node = node.moveRedRight();
        }
        if (key.compareTo(node.key) == 0) {
            deleted = node.value;
            size--;
            Node min = node.right.min();
            node.value = min.value;
            node.key = min.key;
            node.right = removeMin(node.right);
        } else {
            node.right = remove(node.right, key);
        }
        return fixUp(node);
    }
    // end snippet remove
    private boolean isRed(Node node) {
        return node != null && node.color == RED;
    }

    // start snippet fixUp
    private Node fixUp(@NotNull Node node) {
        if (isRed(node.right) && !isRed(node.left)) {
            node = node.rotateLeft();
        }
        if (isRed(node.left) && isRed(node.left.left)) {
            node = node.rotateRight();
        }
        if (isRed(node.left) && isRed(node.right)) {
            node.flipColors();
        }
        return node;
    }
    // end snippet fixUp

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

    // This won't work for larger unbalanced trees (int overflow), but then again you probably
    // wouldn't be displaying them anyway, so this is good enough for now.
    private StringBuilder getDotTreeContent(StringBuilder sb, Node n, int i) {
        if (n == null) {
            return sb.append(String.format("node%d [label=\"NIL\", shape=record, width=.4,height=.25];\n", i));
        } else {
            sb.append(String.format("node%d [label=\"%s\"%s];\n", i, n.key, (n.color == RED ? ",fillcolor=\"#C8242B\",penwidth=0" : "")));
        }
        int leftN = 2 * i;
        int rightN = 2 * i + 1;
        sb.append(String.format("node%d -> node%d;\n", i, leftN));
        getDotTreeContent(sb, n.left, leftN);
        sb.append(String.format("node%d -> node%d;\n", i, rightN));
        getDotTreeContent(sb, n.right, rightN);
        return sb;
    }

    @Override
    public String toString() {
        return "digraph G {\n" +
                "graph [ dpi = 96 ]\n" +
                "nodesep=0.3;\n" +
                "ranksep=0.2;\n" +
                "margin=0.1;\n" +
                "node [shape=ellipse, style=filled, color=black, fontname=XITS, fontcolor=white];\n" +
                "edge [arrowsize=0.8];\n" +
                getDotTreeContent(new StringBuilder(), root, 1) +
                "}";
    }

    private class Node {
        Key key;
        Value value;
        Node left;
        Node right;
        boolean color = RED;

        // Make RED node
        Node(@NotNull Key key, Value value) {
            this.key = key;
            this.value = value;
        }

        void flipColors() {
            color = !color;
            left.color = !left.color;
            right.color = !right.color;
        }

        void setBlack() {
            color = BLACK;
        }

        @NotNull
        private Node min() {
            return left == null ? this : left.min();
        }

        @NotNull
        private Node max() {
            return right == null ? this : right.max();
        }

        @NotNull
        private Node moveRedLeft() {
            flipColors();
            Node node = this;
            if (isRed(right.left)) {
                right = right.rotateRight();
                node = node.rotateLeft();
                node.flipColors();
            }
            return node;
        }

        @NotNull
        private Node moveRedRight() {
            flipColors();
            Node node = this;
            if (isRed(left.left)) {
                node = node.rotateRight();
                node.flipColors();
            }
            return node;
        }

        // start snippet Node-rotateLeft
        @NotNull
        private Node rotateLeft() {
            Node x = right;
            right = x.left;
            x.left = this;
            x.color = color;
            color = RED;
            return x;
        }
        // end snippet Node-rotateLeft

        // start snippet Node-rotateRight
        @NotNull
        private Node rotateRight() {
            Node x = left;
            left = x.right;
            x.right = this;
            x.color = color;
            color = RED;
            return x;
        }
        // end snippet Node-rotateRight
    }
}
