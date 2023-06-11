package ru.spbstu.tgraph;

import org.jetbrains.annotations.NotNull;

// start snippet Set
public interface Set<E> {
    // найти элемент
    boolean contains(@NotNull E element);
    // добавить элемент
    void add(@NotNull E element);
    // удалить элемента
    boolean remove(E element);
    // размер множества
    int size();
    // пустое ли множество
    boolean isEmpty();
    // опустошить множество
    void clear();
}
// end snippet Set