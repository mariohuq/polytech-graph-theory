package ru.spbstu.tgraph;

import org.jetbrains.annotations.NotNull;

public class HashSet<E> implements Set<E> {
    private HashTable<E, Object> map;

    public HashSet() {
        map = new HashTableImpl<>();
    }

    @Override
    public boolean contains(@NotNull E element) {
        return map.containsKey(element);
    }

    // Dummy value to associate with an Object in the backing Map
    private static final Object PRESENT = new Object();

    @Override
    public void add(@NotNull E element) {
        map.put(element, PRESENT);
    }

    @Override
    public boolean remove(E element) {
        return map.remove(element) == PRESENT;
    }

    @Override
    public int size() {
        return map.size();
    }

    @Override
    public boolean isEmpty() {
        return map.isEmpty();
    }

    @Override
    public void clear() {
        map.clear();
    }
}
