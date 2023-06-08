package ru.spbstu.tgraph;

import org.jetbrains.annotations.NotNull;

public interface Set<E> {
    boolean contains(@NotNull E element);

    void add(@NotNull E element);

    boolean remove(E element);

    int size();

    boolean isEmpty();

    void clear();
}
