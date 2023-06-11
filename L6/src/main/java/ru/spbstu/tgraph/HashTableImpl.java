package ru.spbstu.tgraph;

import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

import java.util.Arrays;

public class HashTableImpl<Key, Value> implements HashTable<Key, Value> {

    // start snippet PRIME_CAPACITIES
    private static final int[] PRIME_CAPACITIES = {
            53, 97, 193, 389, 769, 1543, 3079,
            6151, 12289, 24593, 49157, 98317, 196613, 393241,
            786433, 1572869, 3145739, 6291469, 12582917, 25165843, 50331653,
            100663319, 201326611, 402653189, 805306457, 1610612741
    };
    
    private static final int DEFAULT_INITIAL_CAPACITY = PRIME_CAPACITIES[0];
    // end snippet PRIME_CAPACITIES
    private static final double LOAD_FACTOR = 0.75;

    int size;
    Node[] table;
    int capacityIndex = 0;

    public HashTableImpl() {
    }

    // start snippet get
    @Override
    public @Nullable Value get(@NotNull Key key) {
        if (table == null) {
            return null;
        }
        final int hash = key.hashCode();
        final Node bucket = table[index(hash)];
        return bucket == null ? null : bucket.get(hash, key);
    }
    // end snippet get

    // start snippet put
    @Override
    public void put(@NotNull Key key, @NotNull Value value) {
        if (table == null || size >= LOAD_FACTOR * capacity()) {
            table = resize();
        }
        final int hash = key.hashCode();
        final int index = index(hash);
        if (table[index] == null) {
            table[index] = new Node(hash, key, value);
            size++;
            return;
        }
        table[index].put(hash, key, value);
    }
    // end snippet put

    // start snippet remove
    @Override
    public @Nullable Value remove(@NotNull Key key) {
        if (table == null) {
            return null;
        }
        final int hash = key.hashCode();
        final int index = index(hash);
        if (table[index] == null) {
            return null;
        }
        if (table[index].isLast() && table[index].matches(hash, key)) {
            final Value previous = table[index].value;
            table[index] = null;
            size--;
            return previous;
        }
        return table[index].remove(hash, key);
    }
    // end snippet remove
    
    @Override
    public int size() {
        return size;
    }

    @Override
    public boolean isEmpty() {
        return size() == 0;
    }

    @Override
    public void clear() {
        Arrays.fill(table, null);
    }

    private int capacity() {
        return table != null ? table.length : DEFAULT_INITIAL_CAPACITY;
    }

    private int index(int hash) {
        return Math.floorMod(hash, capacity());
    }

    /**
     * Максимальное capacity() == PRIME_CAPACITIES[PRIME_CAPACITIES.length - 1],
     * Если capacity() достигло максимального значения, то дальше table не расширяется.
     */
    // start snippet resize
    private Node[] resize() {
        if (capacityIndex >= PRIME_CAPACITIES.length) {
            return table;
        }
        if (table != null) {
            capacityIndex++;
        }
        final int newCapacity = PRIME_CAPACITIES[capacityIndex];
        // необходимо, так как Node === HashTableImpl<Key, Value>.Node,
        // а массивы с generic типом компонента вне закона (JLS 15.10.1)
        @SuppressWarnings("unchecked")
        final Node[] newTable = (Node[]) new HashTableImpl<?, ?>.Node[newCapacity];
        if (table == null) {
            return newTable;
        }
        for (Node bucket : table) {
            for (Node node = bucket, next; node != null; node = next) {
                next = node.next;
                final int index = Math.floorMod(node.hash, newCapacity);
                node.next = newTable[index];
                newTable[index] = node;
            }
        }
        return newTable;
    }
    // end snippet resize


    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder();
        for (int i = 0; i < table.length; i++) {
            if (table[i] == null) {
                continue;
            }
            sb.append(String.format("[%d] ", i));
            for (Node node = table[i]; node != null; node = node.next) {
                sb.append(" → \"");
                sb.append(node.key);
                sb.append("\"");
            }
            sb.append(" → ∅\n");
        }
        return sb.toString();
    }

    // start snippet Node
    private class Node {
        final Key key;
        final int hash;
        Value value;
        Node next;

        Node(int hash, Key key, Value value, Node next) {
            this.hash = hash;
            this.key = key;
            this.value = value;
            this.next = next;
        }

        Node(int hash, Key key, Value value) {
            this(hash, key, value, null);
        }

        Value get(int hash, Key key) {
            Node node = findOrGetLast(hash, key);
            if (!node.isLast() || node.matches(hash, key)) {
                return node.value;
            }
            return null;
        }

        void put(int hash, Key key, Value value) {
            Node node = this;
            while (!node.isLast() && !node.matches(hash, key)) {
                node = node.next;
            }
            if (!node.isLast() || node.matches(hash, key)) {
                node.value = value;
                return;
            }
            node.next = new Node(hash, key, value);
            size++;
        }

        Value remove(int hash, Key key) {
            Node node = this;
            while (!node.next.isLast() && !node.next.matches(hash, key)) {
                node = node.next;
            }
            if (!node.next.isLast() || node.next.matches(hash, key)) {
                size--;
            }
            Value previous = node.next.value;
            node.next = node.next.next;
            return previous;
        }

        Node findOrGetLast(int hash, Key key) {
            Node node = this;
            while (!node.isLast() && !node.matches(hash, key)) {
                node = node.next;
            }
            return node;
        }

        boolean isLast() {
            return next == null;
        }

        boolean matches(int hash, Key key) {
            return this.hash == hash && this.key.equals(key);
        }
    }
    // end snippet Node
}
