package ru.spbstu.tgraph;

import java.io.File;
import java.io.FileNotFoundException;
import java.util.Scanner;

import static java.lang.System.out;

public class DictionaryApp {
    private final static Scanner userInput = new Scanner(System.in);

    private final Set<String> set;

    public static void main(String[] args) throws FileNotFoundException {
        out.println("Выберите, с какой структурой данных вы хотите работать:");
        new DictionaryApp(switch (askWord()) {
            case "hash" -> new HashSet<>();
            case "rb" -> new RedBlackSet<>();
            default -> null;
        }).main();
    }

    private void main() throws FileNotFoundException {
        if (set == null) {
            out.println("Выбрать можно лишь hash/rb. Попробуйте снова");
            return;
        }
        while (true) {
            switch (askWord()) {
                case "add" -> {
                    out.println("Введите слово, чтобы добавить в словарь:");

                    for (String word : askLine().split("(?U)\\s+")) {
                        set.add(word);
                    }
                    out.printf("В словаре %d эл.\n", set.size());
                }
                case "remove" -> {
                    out.println("Введите слово, чтобы удалить из словаря:");
                    if (set.remove(askWord())) {
                        out.println("Слово удалено из словаря.");
                    } else {
                        out.println("Слова в словаре не было.");
                    }
                }
                case "import" -> {
                    addFromFile();
                    out.printf("В словаре %d эл.\n", set.size());
                }
                case "clear" -> {
                    set.clear();
                    out.printf("В словаре %d эл.\n", set.size());
                }
                case "find" -> {
                    out.println("Введите слово, чтобы найти его в словаре:");
                    if (set.contains(askWord())) {
                        out.println("Слово есть в словаре.");
                    } else {
                        out.println("Слова нет в словаре.");
                    }
                }
                case "show" -> {
                    out.println("Структура:");
                    out.println(set);
                }
                case "exit" -> {
                    return;
                }
                case "?" -> {
                    out.println("Доступные команды:");
                    out.println("add: добавить слово в словарь");
                    out.println("remove: удалить слово из словаря");
                    out.println("import: добавить слова из файла в словарь");
                    out.println("clear: очистить словарь");
                    out.println("find: найти слово в словаре");
                    out.println("exit: выйти");
                }
                default -> out.println("Неизвестная команда. Введите ? чтобы вывести меню");
            }
        }
    }

    public DictionaryApp(Set<String> set) {
        this.set = set;
    }

     // start snippet addFromFile
    private void addFromFile() throws FileNotFoundException {
        out.println("Введите путь до файла без пробелов:");
        Scanner input = new Scanner(new File(askWord())).useDelimiter("(?U)[\\p{Punct}\\s&&[^-']]+");
        input.forEachRemaining(set::add); // прочитать
    }
    // end snippet addFromFile

    private static String askWord() {
        out.print("> ");
        return userInput.next();
    }
    private static String askLine() {
        out.print("> ");
        userInput.skip("\n+");
        return userInput.nextLine();
    }
}