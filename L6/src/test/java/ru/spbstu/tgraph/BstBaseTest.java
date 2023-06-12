package ru.spbstu.tgraph;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertNull;
import static org.junit.jupiter.api.Assertions.assertTrue;

/**
 * Basic binary search tree invariants.
 */
public abstract class BstBaseTest {

    abstract Bst<String, String> newBst();

    @Test
    void emptyBst() {
        Bst<String, String> bst = newBst();
        assertNull(bst.get(""));
        assertNull(bst.get("some key"));
        assertEquals(0, bst.size());
    }

    @Test
    void put() {
        Bst<String, String> bst = newBst();
        bst.put("foo", "bar");

        assertEquals("bar", bst.get("foo"));

        assertEquals(1, bst.size());
    }

    @Test
    void replace() {
        Bst<String, String> bst = newBst();
        bst.put("foo", "bar");
        bst.put("foo", "bee");

        assertEquals("bee", bst.get("foo"));

        assertEquals(1, bst.size());
    }

    @Test
    void morePut() {
        Bst<String, String> bst = newBst();

        int size = 0;
        assertEquals(bst.size(), size);
        assertNull(bst.get("testStringKey1"));

        bst.put("testStringKey1", "testStringValue1");

        assertEquals(bst.size(), ++size);
        assertEquals(bst.get("testStringKey1"), "testStringValue1");

        bst.put("testStringKey2", "testStringValue2");

        assertEquals(bst.size(), ++size);
        assertEquals(bst.get("testStringKey2"), "testStringValue2");

        bst.put("testStringKey2", "case with same value");

        assertEquals(bst.size(), size);
        assertEquals(bst.get("testStringKey2"), "case with same value");

        bst.put("testStringKey3", "testStringValue3");

        assertEquals(bst.size(), ++size);
        assertEquals(bst.get("testStringKey3"), "testStringValue3");

        bst.put("testStringKey", "testStringValue");

        assertEquals(bst.size(), ++size);
        assertEquals(bst.get("testStringKey"), "testStringValue");
    }

    @Test
    void remove() {
        Bst<String, String> bst = newBst();
        assertNull(bst.remove("case when bst is empty"));
        assertTrue(bst.isEmpty());

        bst.put("testStringKey3", "testStringValue3");
        bst.put("testStringKey4", "testStringValue4");
        bst.put("testStringKey2", "testStringValue2");
        bst.put("testStringKey5", "testStringValue5");
        bst.put("testStringKey1", "testStringValue1");
        bst.put("testStringKey0", "testStringValue0");

        assertFalse(bst.isEmpty());
        int size = bst.size();

        assertEquals(bst.remove("testStringKey4"), "testStringValue4");
        assertEquals(bst.size(), --size);
        assertFalse(bst.containsKey("testStringKey4"));

        assertEquals(bst.remove("testStringKey1"), "testStringValue1");
        assertEquals(bst.size(), --size);
        assertFalse(bst.containsKey("testStringKey1"));

        assertNull(bst.remove("testStringKey1"), "testStringValue1");
        assertEquals(bst.size(), size);
        assertFalse(bst.isEmpty());
        assertFalse(bst.containsKey("testStringKey1"));

        assertEquals(bst.remove("testStringKey3"), "testStringValue3");
        assertEquals(bst.size(), --size);
        assertFalse(bst.containsKey("testStringKey3"));

        assertEquals(bst.remove("testStringKey0"), "testStringValue0");
        assertEquals(bst.size(), --size);
        assertFalse(bst.containsKey("testStringKey0"));

        assertEquals(bst.remove("testStringKey2"), "testStringValue2");
        assertEquals(bst.size(), --size);
        assertFalse(bst.containsKey("testStringKey2"));

        assertEquals(bst.remove("testStringKey5"), "testStringValue5");
        assertEquals(bst.size(), --size);
        assertFalse(bst.containsKey("testStringKey5"));

        assertTrue(bst.isEmpty());
    }

    @Test
    void remove_after_remove() {
        Bst<String, String> bst = newBst();
        bst.put("stringKey1", "1");
        bst.remove("stringKey1");
        assertTrue(bst.isEmpty());
        bst.remove("stringKey1");
        assertTrue(bst.isEmpty());
    }


    @Test
    void contains() {
        Bst<String, String> bst = newBst();

        assertFalse(bst.containsKey("testStringKey"));
        assertFalse(bst.containsKey("testStringKey1"));

        bst.put("testStringKey", "testStringValue");
        assertTrue(bst.containsKey("testStringKey"));
        assertFalse(bst.containsKey("testStringKey1"));

        bst.put("testStringKey1", "testStringValue1");
        assertTrue(bst.containsKey("testStringKey1"));
        assertTrue(bst.containsKey("testStringKey"));

        bst.remove("testStringKey");
        assertTrue(bst.containsKey("testStringKey1"));
        assertFalse(bst.containsKey("testStringKey"));

        bst.remove("testStringKey1");
        assertFalse(bst.containsKey("testStringKey"));
        assertFalse(bst.containsKey("testStringKey1"));
    }

    @Test
    void empty() {
        Bst<String, String> bst = newBst();

        assertTrue(bst.isEmpty());

        bst.put("testStringKey", "testStringValue");
        assertFalse(bst.isEmpty());

        bst.put("testStringKey1", "testStringValue1");
        assertFalse(bst.isEmpty());

        bst.remove("testStringKey");
        assertFalse(bst.isEmpty());

        bst.remove("testStringKey1");
        assertTrue(bst.isEmpty());
    }


    @Test
    void moreReplace() {
        Bst<String, String> bst = newBst();

        assertNull(bst.get("1"));

        bst.put("1", "testStringValue3");
        assertEquals(bst.get("1"), "testStringValue3");

        bst.put("1", "testStringValue4");
        assertEquals(bst.get("1"), "testStringValue4");

        bst.put("1", "testStringValue2");
        assertEquals(bst.get("1"), "testStringValue2");

        bst.put("7", "testStringValue5");
        assertEquals(bst.get("7"), "testStringValue5");
        assertEquals(bst.get("1"), "testStringValue2");
    }
}