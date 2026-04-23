# Repository Guidelines

## Project Overview

Cassette is a local music player with a Crow-based C++ web server backend and
React/FluentUI frontend.

## Project Structure

- cpp/ - C++ backend source code
- www/ - React frontend
- build/ - Build outputs

## Build, Test, and Development Commands

- bun run gen - Generate TypeScript & C++ types
- bun run build - Full parallel build
- bun run test - Run CMake tests
- bun run typecheck - TypeScript type checking
- bun run format - Format all code
- bun run clean - Clean build directories
- bun run run - Run the application

## Coding Style & Naming Conventions

**C++:**

- 2-space indentation
- Max 80 characters per line
- Standard: C++20

**TypeScript/JavaScript:**

- 2-space indentation, single quotes
- Max 80 characters per line

## Testing Guidelines

- Run CMake tests with: bun run test
- Tests use GoogleTest framework

## Commit & Pull Request Guidelines

- Use clear, descriptive commit messages
- Link related issues in PR descriptions
- Include screenshots for UI changes
- Run bun run format and bun run test before committing
- Follow existing commit patterns in Git history

## Additional Notes

- The project uses Conan for C++ dependency management
- Build configuration is managed via CMake presets
- All code should maintain consistency with existing TypeScript and C++ patterns
