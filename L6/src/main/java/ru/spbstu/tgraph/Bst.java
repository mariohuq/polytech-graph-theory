package ru.spbstu.tgraph;

import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

/**
 * Binary search tree with ordered operations support.
 */
// start snippet Bst.Bst
public interface Bst<Key extends Comparable<Key>, Value> {
    @Nullable Value get(@NotNull Key key);

    default boolean containsKey(@NotNull Key key) {
        return get(key) != null;
    }

    void put(@NotNull Key key, @NotNull Value value);

    @Nullable Value remove(@NotNull Key key);

    int size();

    default boolean isEmpty() {
        return size() == 0;
    }

    void clear();
}
// end snippet Bst.Bst