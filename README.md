# Ingeniería de Software Científico: Estructuras de Datos y Algoritmos

Bienvenido al repositorio de **Ingeniería de Software Científico** para el curso de Estructuras de Datos y Algoritmos (DSA). Este proyecto no es solo una colección de código; es un entorno de ingeniería diseñado para enseñarte a construir software robusto, escalable y comprobable.

## 📚 Filosofía

Tratamos la Ciencia de la Computación como una ciencia experimental. Cada estructura de datos es una hipótesis sobre eficiencia, y cada benchmark es un experimento para verificarla.

- **Taxonomía**: El código está organizado por concepto matemático, no por capítulo de libro de texto.
- **Higiene**: Aplicamos estándares de codificación estrictos (Google Style) para garantizar la legibilidad y el mantenimiento.
- **Rigor**: No se acepta código sin prueba de corrección (Pruebas Unitarias) y prueba de eficiencia (Benchmarks).

---

## 🚀 Primeros Pasos

### Requisitos previos
- **Compilador de C++** compatible con C++17 (GCC 9+, Clang 10+, MSVC 2019+).
- **CMake** 3.15 o superior.
- **Git** para el control de versiones.

### Instalación

1. **Clona el repositorio**:
   ```bash
   git clone <REPOSITORY_URL>
   cd DSA_Repo
   ```

2. **Configura el proyecto**:
   Usamos CMake para gestionar las dependencias (como GoogleTest) automáticamente.
   ```bash
   cmake -B build -S .
   ```

3. **Compila**:
   ```bash
   cmake --build build
   ```

---

## 🧪 Verificación y Pruebas

Usamos **GoogleTest** para las pruebas unitarias. Debes ejecutar las pruebas con frecuencia.

### Ejecutar las pruebas
Después de compilar, ejecuta la suite de pruebas:
```bash
cd build
ctest --output-on-failure
```
*Consejo: `--output-on-failure` te muestra exactamente por qué falló una prueba.*

### Ejecutar los benchmarks
Para medir el rendimiento de tus implementaciones:
```bash
./build/bench_sorting
# o cualquier otro ejecutable de benchmark
```

---

## 🗺️ Módulos del curso

Este repo se construye por hitos, uno por estructura de datos. El roadmap
completo (qué falta implementar en cada módulo, qué guía leer primero) vive
en **[docs/MILESTONES.md](docs/MILESTONES.md)**.

`main` es la versión **esqueleto**: las firmas, la documentación Doxygen y
los tests ya existen, pero los métodos clave de cada estructura están
marcados `// TODO(estudiante): ...` y lanzan `std::logic_error` hasta que se
implementan — por eso la suite de tests parte en rojo (73/341 en verde de
entrada: todo el andamiaje que ya funciona). Cada test que falla es, en
efecto, un ítem de la lista de tareas. La rama `solutions` tiene la
implementación de referencia completa (uso del docente).

---

## 📂 Estructura del proyecto

El repositorio sigue una separación de responsabilidades clara:

```text
.
├── include/dsa/        # LIBRERÍA HEADER-ONLY (el núcleo)
│   ├── interfaces/     # Clases base abstractas (contratos)
│   ├── linear/         # Arreglos, listas, pilas, colas, deque
│   ├── trees/          # BST, árbol binario, heap, cola de prioridad
│   ├── hashing/         # HashMap, HashSet
│   ├── graphs/          # Grafo, BFS/DFS/Dijkstra/orden topológico
│   ├── algorithms/     # Ordenamiento, búsqueda
│   └── utils/          # Utilidades (Timer, Random, etc.)
├── docs/guides/         # Guías pedagógicas por módulo (español)
├── tests/              # Pruebas unitarias (GoogleTest)
├── benchmarks/         # Scripts de medición de rendimiento
├── examples/           # Comprobaciones mínimas tipo "Hello World"
└── .github/            # Configuración de CI/CD (calificación automática)
```

---

## 🤝 Cómo contribuir

Para aprobar este curso, debes actuar como un Ingeniero de Software contribuyendo a un gran proyecto de código abierto.

### Flujo de trabajo

1. **Fork y rama**:
   Nunca trabajes sobre `main`. Crea una rama de funcionalidad para tu tarea.
   ```bash
   git checkout -b feat/linked-list-implementation
   ```

2. **Desarrollo guiado por pruebas (TDD)**:
   - **Paso 1**: Escribe una prueba que falle en `tests/`.
   - **Paso 2**: Define la interfaz en `include/dsa/`.
   - **Paso 3**: Implementa la lógica.
   - **Paso 4**: Verifica que la prueba pase.

3. **Higiene del código**:
   Antes de hacer commit, asegúrate de que tu código esté formateado correctamente. Usamos `Clang-Format`.
   ```bash
   # Si tienes clang-format instalado
   clang-format -i include/dsa/your_file.hpp
   ```

4. **Commit**:
   Escribe mensajes de commit claros y descriptivos.
   ```bash
   git commit -m "Feat: Implement DoublyLinkedList insertion logic"
   ```

5. **Pull Request (PR)**:
   - Sube tu rama a GitHub.
   - Abre un PR contra `main`.
   - **Revisa el estado del CI**: Espera la marca de verificación. Si ves una cruz roja ❌, tu código falló al compilar o al pasar las pruebas en el servidor. ¡Corrígelo!

### Reglas de convivencia
- **Header-Only**: La mayoría de las estructuras de datos que usan plantillas (templates) deben estar en archivos `.hpp` dentro de `include/dsa`.
- **Sin binarios**: Nunca hagas commit de archivos `.exe`, `.o`, ni del directorio `build/`. (El `.gitignore` se encarga de esto, no lo evites).
- **Documentación**: Comenta la lógica compleja. "El código te dice el cómo, los comentarios te dicen el porqué."

---
*¡Feliz codificación!*
