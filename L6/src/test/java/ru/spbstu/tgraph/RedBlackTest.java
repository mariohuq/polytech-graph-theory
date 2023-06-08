package ru.spbstu.tgraph;

public final class RedBlackTest extends BstBaseTest {
    @Override
    Bst<String, String> newBst() {
        return new RedBlackBst<>();
    }

}
