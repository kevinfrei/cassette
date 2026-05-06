# Repository Guidelines

## Project Overview

Cassette is a local music player with a Crow-based C++ web server backend and
React/FluentUI frontend.

## Project Structure

- src/ - C++ backend source code
- www/ - React frontend
- build/ - Build outputs

## Build, Test, and Development Commands

- bun run gen - Generate TypeScript & C++ interoperable/shared types
- bun run clean - Clean the build (requires `bun run cfg` afterward)
-
- bun run cfg - Configure the C++ build (Debug)
- bun run dcfg - Configure the C++ build (Debug)
- bun run rcfg - Configure the C++ build (Release)
-
- bun run cpp - Build the C++ code (Debug)
- bun run dcpp - Build the C++ code (Debug)
- bun run rcpp - Build the C++ code (Release)
-
- bun run www - Build & package the web code (Debug)
- bun run dwww - Build & package the web code (Debug)
- bun run rwww - Build & package the web code (Release)
-
- bun run bld - Full parallel build (Debug)
- bun run dbld - Full parallel build (Debug)
- bun run rbld - Full parallel build (Release)
-
- bun run tst - Run CMake tests (Debug)
- bun run dtst - Run CMake tests (Debug)
- bun run rtst - Run CMake tests (Release)
-
- bun run run - Run the application (Debug)
- bun run drun - Run the application (Debug)
- bun run rrun - Run the application (Release)
-
- bun run br - Build & run the application (Debug)
- bun run dbr - Build & run the application (Debug)
- bun run rbr - Build & run the application (Release)
-
- bun run typecheck - TypeScript type checking
- bun run format - Format all code

## Coding Style & Naming Conventions

- Core conventions are enforced by Prettier, Clang-Format, and Passable (using
  `simple-git-hooks`)
  - 2-space indentation
  - Max 80 characters per line
  - Braces on the same line

**C++:**

- Standard: C++23 without modules (yet: Waiting for import std to be widely
  supported)
- snake_case'd everything, pretty much
- NO `using`s IN HEADERS!
- Braces required (no `if (foo) bar();`)
- Don't use the preprocessor (getting ready for modules!)
- Standard includes, then external includes, then user includes, then self
  include, separated by double eol

**TypeScript:**

- PascalCase'd exports, camelCased local stuff
- No Javascript (.ts/.tsx only!)
- No `any`
- Use `unknown` where necessary, and use `@freik/typechk` to validate types
- 2-space indentation, single quotes
- Max 80 characters per line

## Testing Guidelines

- Run CMake tests with: bun run test
- C++ tests use GoogleTest framework
- Web tests use the bun test runner

## Commit & Pull Request Guidelines

- Use clear, descriptive commit messages
- Link related issues in PR descriptions
- Include screenshots for UI changes
- Run bun run format and bun run test before committing (`simple-git-hooks`
  should enforce this)

## Additional Notes

- The project uses Conan for C++ dependency management: Add libraries to the
  `conanfile.py` file
- Build configuration is managed via CMake presets
- All code should maintain consistency with existing TypeScript and C++ patterns
