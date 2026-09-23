# Configuración para estudiantes

Esta guía explica cómo obtener tu propia copia de la librería para trabajar
durante el curso, sin necesidad de hacer un *fork* público.

## 1. Crea tu repositorio a partir de la plantilla

1. Entra a [github.com/luisedseijas/DSA_Scientific_Computing](https://github.com/luisedseijas/DSA_Scientific_Computing).
2. Haz clic en el botón verde **"Use this template" → "Create a new repository"**
   (arriba a la derecha, junto a "Code"). **No uses "Fork"** — un fork queda
   públicamente vinculado al repo original y no se puede volver privado
   gratis; "Use this template" crea una copia independiente.
3. Elige un nombre, por ejemplo `dsa-<tu-usuario-o-nombre>` (ej. `dsa-jperez`).
4. Marca el repositorio como **Private**.
5. Crea el repositorio.

## 2. Compárteme el acceso

1. En tu nuevo repo: **Settings → Collaborators → Add people**.
2. Agrega al usuario `luisedseijas` (o al usuario/correo que te indique el
   profesor) como colaborador.
3. Pega el link de tu repositorio en el formulario/hoja de cálculo que se
   compartirá en el primer módulo del curso. Ese es el registro que se usa
   para hacer seguimiento y calificar.

## 3. Clona tu repo y verifica que compila

```bash
git clone <URL-de-tu-repo-privado>
cd <tu-repo>
cmake -B build -S .
cmake --build build
./build/unit_tests
```

Al inicio deberías ver **73 de 341 tests en verde** — es el andamiaje que ya
funciona (constructores, iteradores, accesores). El resto son tu lista de
tareas: cada módulo tiene sus `// TODO(estudiante): ...` en
`include/dsa/<categoria>/*.hpp`.

## 4. Flujo de trabajo por módulo

1. Lee la guía del módulo en `docs/guides/NN_*.md` (ver el orden completo en
   [docs/MILESTONES.md](MILESTONES.md)).
2. Revisa el test correspondiente en `tests/<categoria>/*_test.cpp` — define
   exactamente qué debe cumplir tu implementación.
3. Implementa los `TODO` en el header del módulo.
4. Verifica localmente:
   ```bash
   cmake --build build && ./build/unit_tests --gtest_filter="*NombreDelModulo*"
   ```
5. Haz commit y push a tu propio repo. GitHub Actions correrá la suite
   completa automáticamente (pestaña **Actions** de tu repo) — así puedes
   ver tu progreso sin depender de que el profesor revise manualmente.

## Preguntas frecuentes

**¿Puedo trabajar en pareja?** Si el profesor lo permite, uno de los dos crea
el repo desde la plantilla y agrega al otro como colaborador además del
profesor.

**¿Qué pasa si mi código no compila?** Revisa el log de GitHub Actions
(pestaña Actions → el workflow más reciente → "Run Tests"), ahí se ve el
error de compilación exacto.

**¿Puedo ver la solución completa?** No — la rama de referencia es privada y
de uso exclusivo del profesor. Si llevas varios días atascado en un mismo
`TODO`, pregunta en clase o por el canal del curso.
