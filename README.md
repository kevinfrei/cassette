# <img src="web/img/icon.svg" height="30"/>uark

Cuark is my personal foundation template for cross-platform, compact,
performant, native apps. It's a lightweight, C++-powered template (not a
library) for building desktop applications with a Web UI, because who wants to
deal with GTK **and** learn Swift **and** figure out the Windows UI XAML crap or
go _anywhere near_ QT. Cuark provides a high-performance core that
cross-platform apps need without the performance and packaging bloat of
Electron.

I'm using [Crow](https://crowcpp.org) as a local C++ web server, with a _very_
simple IDL (interface definition language) that can produce both C++ and
TypeScript types and automatic (typed) JSON marshalling/unmarshalling code. Any
additional libraries needed should be included using [Conan](https://conan.io)
in the [conanfile.py](conanfile.py) python (in the 'Libraries' list).

## Why Cuark?

- 🚀 Minimal Mass:

  No bundled Chromium instances. My apps start faster and are _dramatically_
  smaller than their electron equivalents.

- ⚙️ C++ Core:

  I write all complex, back-end logic in [C++20](https://cppstd20.com/), built
  with [CMake](https://cmake.org), [Conan](https://conan.io/center), and
  [GTest](https://google.github.io/googletest/).

- ⚡ Bun Integration:

  I use [bun](https://bun.com), a fast, all-inclusive JS/TS package manager,
  runtime, bundler, and test runner to power my frontend logic as well as most
  of the build/configuration scripts. There's not a weird custom environment to
  learn. It's all just Typescript...

- 🛠️ Development First:

  This isn't a framework, but a template. I clone it, compile it, and I'm ready
  to build my stuff.

## What is this?

_(or perhaps)_

## What possessed me to make one of these things?

I spent 30 years as a very specific kind of software developer (compilers...),
but while I was at Microsoft, I slowly picked up the ability to build simple UI
apps using WPF (it's messy and ugly: don't bother). After spending a long time
at Microsoft, I moved to Meta (Facebook at the time) and was regularly annoyed
at not being able to build simple UI apps to run on my Mac. This happened around
the time that React was made publicly available, so I learned enough React to be
dangerous, and then discovered Electron.

Fast forward 10 or 12 years: Electron is a pig. It's also annoying & slow as a
developer. Every developer step is slow & clunky. The fact that all the code
runs in NodeJS is pretty frustrating, as it's pretty slow, and there's set of
things it's just bad at. That said, I regularly switch between Windows, macOS,
and Linux, and want to have my stuff work on all three. So I started looking for
alternatives that would enable me to keep using the stuff I already know.

The first, and most obvious alternative is [Tauri](https://tauri.app). It looks
like a reasonably decent app framework, but it had two problems: First, I don't
wanna learn Rust. I've tried a couple times, but as a language, it has too many
weird warts (|| syntax for lambdas?) and I'm reasonably decent in C++ (and
honestly, C++20 or later is pretty easy to 'stay safe' in). Secondly, as a
runtime, the Tauri folks are "Security first" almost to a fault which put my
original project in a bind, because they wanted to be able to fully validate a
FLAC audio file before adding the ability to tell the runtime that a given
binary stream is FLAC audio. No go for my silly personal app, so I wasn't going
to make the Rust jump but still need Electron for my music player.

Next, I wound up looking at [Neutralino](https://neutralino.js.org). I believe
it would do what I'd need it to do, but it has a a couple of annoying things.
First, I don't really need any extension to the existing browser JS runtime,
thanks. I just need a callback into native code. I like the idea of being able
to run the front end on a much dumber or more restrictive device that my PC
(Android TV? iPhone?) while just running the back-end as a service. (I haven't
even tried any of this, but it's maybe something I'll try in the future?) Scond,
OMG, that 'build tool'?!? WHAT IN THE NAME OF ALL THINGS HOLY WOULD CAUSE ANYONE
TO CREATE A NEW C++ BUILD SYSTEM AFTER 2005?!?! **Just.Use.CMake.** It ain't
perfect but it's better than the pile of horse-manure that 'buildzri' is...

Sorry. I got a little triggered there. Back to the primary narrative:

So, anyway, I decided to create my own little thin wrapper around a webview. The
two primary goals are

1. Letting me make UI apps
2. Being small/lightweight

Anyway, I'll hopefully try to improve this over time. But this is what's here
for now.
