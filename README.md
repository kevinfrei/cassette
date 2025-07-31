# Cassette

## A local music player from inside the browser

I've gotten frustrated with just how bloated and slow Electron was, and a friend
pointed me at the [CrowCPP](https://crowcpp.org) C++ web server. I decided to
take a swipe at porting my
[Electron Music Player](https://github.com/kevinfrei/EMP) over to a strictly
browser-hosted solution with a crow-based back end.

Currently, it's not working, but I'm actively pushing it toward completion. I
had to migrate off of [Recoil](https://recoiljs.org) because Meta's definitely
abandoned it, so I'm moving to [Jotai](https://jotai.org) as part of this
migration. I'd say things are about 50% done.

I did put together a little [IDL](https://github.com/kevinfrei/crow-idl) to
auto-generate common, shared types between Typescript and C++. Getting it up and
going has been a fun little past-time.
