# Scientific Software Engineering: Data Structures & Algorithms

Welcome to the **Scientific Software Engineering** repository for the Data Structures and Algorithms (DSA) course. This project is not just a collection of code; it is an engineering environment designed to teach you how to build robust, scalable, and testable software.

## 📚 Philosophy

We treat Computer Science as an experimental science. Every data structure is a hypothesis about efficiency, and every benchmark is an experiment to verify it.

- **Taxonomy**: Code is organized by mathematical concept, not by textbook chapter.
- **Hygiene**: We enforce strict coding standards (Google Style) to ensure readability and maintainability.
- **Rigor**: No code is accepted without proof of correctness (Unit Tests) and proof of efficiency (Benchmarks).

---

## 🚀 Getting Started

### Prerequisites
- **C++ Compiler** supporting C++17 (GCC 9+, Clang 10+, MSVC 2019+).
- **CMake** 3.15 or higher.
- **Git** for version control.

### Installation

1. **Clone the repository**:
   ```bash
   git clone <REPOSITORY_URL>
   cd DSA_Repo
   ```

2. **Configure the project**:
   We use CMake to manage dependencies (like GoogleTest) automatically.
   ```bash
   cmake -B build -S .
   ```

3. **Build**:
   ```bash
   cmake --build build
   ```

---

## 🧪 Verification & Testing

We use **GoogleTest** for unit testing. You should run tests frequently.

### Running Tests
After building, run the test suite:
```bash
cd build
ctest --output-on-failure
```
*Tip: `--output-on-failure` shows you exactly why a test failed.*

### Running Benchmarks
To measure the performance of your implementations:
```bash
./build/bench_sorting
# or any other benchmark executable
```

---

## 📂 Project Structure

The repository follows a clean separation of concerns:

```text
.
├── include/dsa/        # HEADER-ONLY LIBRARY (The Core)
│   ├── interfaces/     # Abstract base classes (Contacts)
│   ├── linear/         # Arrays, Lists, Stacks, Queues
│   ├── trees/          # BST, AVL, RB-Trees, Heaps
│   ├── algorithms/     # Sorting, Searching
│   └── utils/          # Helpers (Timer, Random, etc.)
├── tests/              # Unit tests (GoogleTest)
├── benchmarks/         # Performance measurement scripts
├── examples/           # Minimal "Hello World" checks
└── .github/            # CI/CD Configuration (Automated Grading)
```

---

## 🤝 How to Contribute

To pass this course, you must act like a Software Engineer contributing to a large open-source project.

### Workflow

1. **Fork & Branch**:
   Never work on `main`. Create a feature branch for your assignment.
   ```bash
   git checkout -b feat/linked-list-implementation
   ```

2. **Test-Driven Development (TDD)**:
   - **Step 1**: Write a failing test in `tests/`.
   - **Step 2**: Define the interface in `include/dsa/`.
   - **Step 3**: Implement the logic.
   - **Step 4**: Verify the test passes.

3. **Code Hygiene**:
   Before committing, ensure your code is formatted correctly. We use `Clang-Format`.
   ```bash
   # If you have clang-format installed
   clang-format -i include/dsa/your_file.hpp
   ```

4. **Commit**:
   Write clear, descriptive commit messages.
   ```bash
   git commit -m "Feat: Implement DoublyLinkedList insertion logic"
   ```

5. **Pull Request (PR)**:
   - Push your branch to GitHub.
   - Open a PR against `main`.
   - **Check CI Status**: Wait for the checkmark. If you see a red cross ❌, your code failed to compile or pass tests on the server. Fix it!

### Rules of Engagement
- **Header-Only**: Most data structures using Templates must be in `.hpp` files in `include/dsa`.
- **No Binaries**: Never commit `.exe` checks, `.o` files, or the `build/` directory. (The `.gitignore` handles this, do not bypass it).
- **Documentation**: Comment complex logic. "Code tells you how, comments tell you why."

---
*Happy Coding!*
