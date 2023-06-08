plugins {
    id("java")
}

group = "ru.spbstu.tgraph"
version = "1.0-SNAPSHOT"

repositories {
    mavenCentral()
}

dependencies {
    // Annotation
    implementation ("org.jetbrains:annotations:16.0.3")

    // JUnit Jupiter test framework
    testImplementation(platform("org.junit:junit-bom:5.9.1"))
    testImplementation("org.junit.jupiter:junit-jupiter")

    testImplementation("org.apache.commons:commons-lang3:3.11")
}

tasks.test {
    maxHeapSize = "128m"
    useJUnitPlatform()
}