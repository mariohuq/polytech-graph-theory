package ru.spbstu.tgraph;

public final class AvlTest extends BstBaseTest {
    @Override
    Bst<String, String> newBst() {
        return new AvlBst<>();
    }
}
