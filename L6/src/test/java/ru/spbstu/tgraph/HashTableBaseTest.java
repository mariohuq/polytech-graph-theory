package ru.spbstu.tgraph;

import java.util.HashMap;
import java.util.Map;
import java.util.Objects;

import org.apache.commons.lang3.RandomStringUtils;
import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertNull;
import static org.junit.jupiter.api.Assertions.assertTrue;

/**
 * Basic tests on {@link HashTable}.
 */
class HashTableBaseTest {

    // Intentionally non-comparable
    static class Key {
        final String value;

        public Key(String value) {
            this.value = value;
        }

        @Override
        public boolean equals(Object o) {
            if (this == o) return true;
            if (o == null || getClass() != o.getClass()) return false;
            Key key = (Key) o;
            return Objects.equals(value, key.value);
        }

        @Override
        public int hashCode() {
            return Objects.hash(value);
        }
    }

    // Has the same hashCode for each instance
    static class AntiKey {
        @Override
        public int hashCode() {
            return 0x314;
        }
    }

    HashTable<String, String> newTable() {
        return new HashTableImpl<>();
    }

    HashTable<Key, String> newStrangeKeyTable() {
        return new HashTableImpl<>();
    }

    HashTable<AntiKey, String> newAntiKeyTable() {
        return new HashTableImpl<>();
    }

    @Test
    void emptyTable() {
        HashTable<String, String> table = newTable();
        assertNull(table.get(""));
        assertNull(table.get("some key"));
        assertEquals(0, table.size());
    }


    @Test
    void put() {
        HashTable<String, String> table = newTable();

        int size = 0;
        assertEquals(table.size(), size);
        assertNull(table.get("testStringKey1"));

        table.put("testStringKey1", "testStringValue1");

        assertEquals(table.size(), ++size);
        assertEquals(table.get("testStringKey1"), "testStringValue1");

        table.put("testStringKey2", "testStringValue2");

        assertEquals(table.size(), ++size);
        assertEquals(table.get("testStringKey2"), "testStringValue2");

        table.put("testStringKey2", "case with same value");

        assertEquals(table.size(), size);
        assertEquals(table.get("testStringKey2"), "case with same value");

        table.put("testStringKey3", "testStringValue3");

        assertEquals(table.size(), ++size);
        assertEquals(table.get("testStringKey3"), "testStringValue3");

        table.put("testStringKey", "testStringValue");

        assertEquals(table.size(), ++size);
        assertEquals(table.get("testStringKey"), "testStringValue");
    }

    @Test
    void remove() {
        HashTable<String, String> table = newTable();
        assertNull(table.remove("case when HashTable is empty"));
        assertTrue(table.isEmpty());

        table.put("testStringKey3", "testStringValue3");
        table.put("testStringKey4", "testStringValue4");
        table.put("testStringKey2", "testStringValue2");
        table.put("testStringKey5", "testStringValue5");
        table.put("testStringKey1", "testStringValue1");
        table.put("testStringKey0", "testStringValue0");

        assertFalse(table.isEmpty());
        int size = table.size();

        assertEquals(table.remove("testStringKey4"), "testStringValue4");
        assertEquals(table.size(), --size);
        assertFalse(table.containsKey("testStringKey4"));

        assertEquals(table.remove("testStringKey1"), "testStringValue1");
        assertEquals(table.size(), --size);
        assertFalse(table.containsKey("testStringKey1"));

        assertNull(table.remove("testStringKey1"), "testStringValue1");
        assertEquals(table.size(), size);
        assertFalse(table.isEmpty());
        assertFalse(table.containsKey("testStringKey1"));

        assertEquals(table.remove("testStringKey3"), "testStringValue3");
        assertEquals(table.size(), --size);
        assertFalse(table.containsKey("testStringKey3"));

        assertEquals(table.remove("testStringKey0"), "testStringValue0");
        assertEquals(table.size(), --size);
        assertFalse(table.containsKey("testStringKey0"));

        assertEquals(table.remove("testStringKey2"), "testStringValue2");
        assertEquals(table.size(), --size);
        assertFalse(table.containsKey("testStringKey2"));

        assertEquals(table.remove("testStringKey5"), "testStringValue5");
        assertEquals(table.size(), --size);
        assertFalse(table.containsKey("testStringKey5"));

        assertTrue(table.isEmpty());
    }

    @Test
    void remove_after_remove() {
        HashTable<String, String> ht = newTable();
        ht.put("stringKey1", "1");
        ht.remove("stringKey1");
        assertTrue(ht.isEmpty());
        ht.remove("stringKey1");
        assertTrue(ht.isEmpty());
    }


    @Test
    void contains() {
        HashTable<String, String> table = newTable();

        assertFalse(table.containsKey("testStringKey"));
        assertFalse(table.containsKey("testStringKey1"));

        table.put("testStringKey", "testStringValue");
        assertTrue(table.containsKey("testStringKey"));
        assertFalse(table.containsKey("testStringKey1"));

        table.put("testStringKey1", "testStringValue1");
        assertTrue(table.containsKey("testStringKey1"));
        assertTrue(table.containsKey("testStringKey"));

        table.remove("testStringKey");
        assertTrue(table.containsKey("testStringKey1"));
        assertFalse(table.containsKey("testStringKey"));

        table.remove("testStringKey1");
        assertFalse(table.containsKey("testStringKey"));
        assertFalse(table.containsKey("testStringKey1"));
    }

    @Test
    void empty() {
        HashTable<String, String> table = newTable();

        assertTrue(table.isEmpty());

        table.put("testStringKey", "testStringValue");
        assertFalse(table.isEmpty());

        table.put("testStringKey1", "testStringValue1");
        assertFalse(table.isEmpty());

        table.remove("testStringKey");
        assertFalse(table.isEmpty());

        table.remove("testStringKey1");
        assertTrue(table.isEmpty());
    }

    @Test
    void replace() {
        HashTable<String, String> table = newTable();

        assertNull(table.get("1"));

        table.put("1", "testStringValue3");
        assertEquals(table.get("1"), "testStringValue3");

        table.put("1", "testStringValue4");
        assertEquals(table.get("1"), "testStringValue4");

        table.put("1", "testStringValue2");
        assertEquals(table.get("1"), "testStringValue2");

        table.put("7", "testStringValue5");
        assertEquals(table.get("7"), "testStringValue5");
        assertEquals(table.get("1"), "testStringValue2");
    }

    @Test
    void equalsUsed() {
        HashTable<String, String> table = newTable();

        assertNull(table.get("1"));

        table.put(new String("1"), "testStringValue3");
        assertEquals(table.get(new String("1")), "testStringValue3");

        table.put(new String("1"), "testStringValue4");
        assertEquals(table.get("1"), "testStringValue4");

        table.put(new String("1"), "testStringValue2");
        assertEquals(table.get(new String("1")), "testStringValue2");

        table.put(new String("7"), "testStringValue5");
        assertEquals(table.get(new String("7")), "testStringValue5");
        assertEquals(table.get(new String("1")), "testStringValue2");

        table.remove(new String("7"));
        assertNull(table.get(new String("7")));
    }

    @Test
    void notComparableKey() {
        HashTable<Key, String> table = newStrangeKeyTable();
        HashMap<Key, String> reference = new HashMap<>();
        for (int i = 0; i < 10000; i++) {
            Key key = new Key(RandomStringUtils.random(3));
            String value = RandomStringUtils.random(100);
            table.put(key, value);
            reference.put(key, value);
        }
        assertEquals(reference.size(), table.size());
        for (Map.Entry<Key, String> entry: reference.entrySet()) {
            assertEquals(entry.getValue(), table.get(new Key(entry.getKey().value)));
        }
    }

    @Test
    void manyCollisions() {
        HashTable<Key, String> table = newStrangeKeyTable();
        HashMap<Key, String> reference = new HashMap<>();
        Key key = new Key("1");
        for (int i = 0; i < 10000; i++) {
            String value = RandomStringUtils.random(100);
            table.put(key, value);
            reference.put(key, value);
        }
        assertEquals(reference.size(), table.size());
        for (Map.Entry<Key, String> entry: reference.entrySet()) {
            assertEquals(entry.getValue(), table.get(new Key(entry.getKey().value)));
        }
    }

    @Test
    void removeWithCollision() {
        var table = newAntiKeyTable();
        var key0 = new AntiKey();
        var key1 = new AntiKey();
        var key2 = new AntiKey();

        table.put(key0, "0");
        assertNull(table.remove(key1));
        table.put(key1, "1");
        assertNull(table.remove(key2));
        assertEquals("0", table.remove(key0));
        assertEquals("1", table.remove(key1));
        assertTrue(table.isEmpty());
    }
}