# Lantern Hall

## The invitation

In the winter of 2001, a group of friends dragged their computers through the snow to play in a municipal hall. One chair always stood empty for whoever arrived late. Twenty-five years later a dot-matrix printer spits out a new invitation. The paper is still warm. The hall is lit, the folding tables are set, and a machine in the cloakroom is waiting for 24 people to press Ready.

A player can spend December finding out who left that invitation. They can also knock on a friend’s door on December 17, borrow a controller, and play that night’s adventure together. Both are complete ways to enjoy the game.

**Form.** A first-person co-op advent calendar. Twenty-four self-contained missions take about twenty minutes apiece, for roughly eight hours on the main path. The hub is safe, warm and always open. The missions borrow the decisions people remember making in older games. Every place, character, prop, tune, map and line of dialogue is made for Lantern Hall.

## A night at the hall

You arrive at the south doors with snow on your sleeves. Down the runner, an evergreen stands beneath a glass star. Two rows of twelve numbered arches occupy the sides of the nave. Brass dates can be read at walking distance. Each dark arch has a frosted shutter stamped with a date. A ready arch leaks light onto the floor and hums a two-note invitation. A completed arch carries a ribbon, and its table setting gains an object you can pick up and turn over.

The first time you enter, follow an extension lead to the cloakroom printer. It prints “Please leave one place free.” From there you can walk directly into a lit arch. Looking at its brass plate reveals the premise, expected time, signature tools and friends inside. A wall almanac offers the same route by number for players who find the walk difficult. Meeting at the table, reading previous postcards and replaying open doors are always optional.

In a mission, run, look, jump, point a tool, fire it or interact. Each door loans two or three signature mechanics. One safe encounter teaches each, one situation combines them, and one last choice asks the player to improvise. Returning the keepsake clears the door. Back in the hall the reward is visible to the whole party. Exit, retry or suspend at a marked bench without losing an evening’s earlier progress.

## The mystery at the long table

| December | What happens in the rooms | What changes in the hall |
| --- | --- | --- |
| 1–6 · Set the table | Ordinary city workers, pilots and builders have been told to prepare places for guests who never arrive. Each keepsake bears the same crooked punch mark. | Six settings reappear. The projector plays a silent film of the 2001 hall. The person operating the camera has no seat in the shot. |
| 7–12 · Trace the cable | Invitations have been routed through relay stations, a mine and an underground post office. The routing slip says HOLD UNTIL EVERYONE IS HERE. | A cable lights up under the runner. The cloakroom machine, called the Steward, reads the instruction in the voice of the friend who wrote it. |
| 13–18 · Read the margins | Each world preserved a different workaround for one person being late. Signal fires, rescue teams and a borrowed chair kept games in motion. The author’s handwritten correction reads “start anyway”. | Players assemble a second recording. The old program mistook a promise to keep a seat for a command to freeze the evening. |
| 19–24 · Send the invitation | People inside the archives help build a return route. The team can invite them into the hall even if a guest misses a day. The final mission sends invitations to every December evening. | The clock passes midnight for the first time in 25 years. The final chair stays empty by choice. Friends arriving after Christmas can still sit down and play. |

A dated door never requires yesterday’s keepsake. Each chapter supplies the one-sentence context needed to play it first. Story fragments assemble by date in the guestbook. Milestone films appear as their six pieces arrive. Day 24 always delivers the ending, and the full guestbook adds a short second recording without changing access or rewards. Nobody has to log in on Christmas Eve.

## An evening in detail · Door 01, House Lights

A paper ticket protrudes from a closed cinema. Behind the glass an enormous cardboard usher politely tears the same ticket over and over. The only way to read the invitation printed on the house reel is to get the projector running.

| Full mission time | What the player sees and does | What the encounter teaches |
| --- | --- | --- |
| 0–3 min | Follow the flickering marquee from a snowed-in tram stop. Use the ticket punch to release the side door. A reel silhouette and the sound of a film skipping establish the destination. | Everyday fixtures respond to the same use button as doors. |
| 3–8 min | Test the pocket projector on a cardboard cutout. Fold a second usher blocking the sweets counter, then pass its narrow turnstile. A held pulse also folds hostile scenery. | The tool changes scale. Its targets have a pleated outline visible without colour. |
| 8–13 min | Recover the lift pack in the orchestra pit. Practice on a low catwalk before crossing the missing stairs to the booth. Safe ledges top up the pack. | Reading heights and spending fuel outrank precise jumping. A missed landing drops you beside the route. |
| 13–17 min | Thread the show reel at the booth. A second player can draw ushers into the projection beam or shrink them from the balcony. A solo player parks the beam with a latch. | Shrinking, flight and operating the cinema now serve the same objective. |
| 17–20 min | Return through the auditorium while the recovered film plays on the screen. Punch the ticket at the alley exit. Listen to the first new sound in the hall, a folding chair scraping the floor. | The return route makes you see the same room differently. |

The public rehearsal compresses this into a small auditorium. Shrink two cardboard ushers, boost to the booth, start the projector and return its ticket to the entrance. The full campaign includes the street, pit, changing encounters and narrated return.

## Playing with friends

The campaign target is one to four expedition members, with two local views on one Windows, Linux or web device. A pair on one sofa will ultimately be able to occupy two seats in an online party. Local split chooses side-by-side in landscape and stacked views in portrait. Each view has its own reticle, prompt and subtitles. A shared table, doors and mission state make opening a portal or solving a puzzle a team action.

A host chooses a door. Nearby friends can join immediately or meet the host at the next bench. Enemies gain additional roles or approaches with more players, with a cap on pressure per player. Puzzle switches latch for a solo player. A helper drone handles a held action if a partner disconnects. Catch-up uses the host’s calendar and records credit for late joiners who reach extraction. Guests keep their own completion ledger. Friends can replay any open evening together.

The early browser build is sized for two players and tests one shared scene at a time. Same-keyboard or touch-plus-keyboard split, a direct browser invitation and an optional WebSocket relay exercise the shared simulation. The release calls for a hosted room service, late join, reconnection, mixed local/online teams and native cross-play. [See the platform and verification design.](engineering.md)

## Art, sound and comfort

* **A readable room from either end.** The nave has a burgundy woven runner, tall green arches, beeswax lights, brass date plates and a blue rose window. Warm light marks social space. Cold light marks an unanswered invitation. The three portal states also differ by shape and text.
* **Three kinds of image.** Low-poly geometry gives walls, moving doors and railings a reliable silhouette. Directional sprites supply paper people, sparks and cloth. Small voxel models give keepsakes, gifts and the projector tactile edges when examined up close. Keep UI type at display resolution over a nearest-neighbour world image.
* **Candlelight that moves.** Bake broad ambient values into surfaces. Add a small number of local, moving lights, contact shadows and moving occlusion. Author a palette card and one recognisable silhouette per door. The 3D translation of an isometric game borrows its silhouettes, unit colours and resource decisions. Keep the camera at eye level.
* **Something to listen for.** A relay clicks at each new unlock. The same four-note table tune returns as an industrial fan rhythm, a handbell phrase and a station announcement. Make new melodies, Foley and dialogue. Give each sound that conveys information a caption and a visible counterpart.
* **An evening people can finish.** Remappable inputs, toggled boost, subtitles with speaker names, scalable type, left-handed touch, reduced flashes, no obligatory head bob and adjustable difficulty are release criteria. VR defaults to snap turn, a level horizon, a seated origin and a comfort vignette. Carpet flight and six-axis caves offer horizon lock and marked route anchors.

## What constitutes a finished door

Deliver a readable entrance, a safe first use for each signature tool, two distinct encounter spaces, one combination puzzle, a return route, a keepsake, solo and co-op solutions, a mid-mission bench and a captioned hall payoff. Playtest with newcomers and one returning fan. Target a 17–23 minute median first clear excluding lobby time, with a shorter assisted route and optional secrets off the critical path. Record retries, time spent lost and how many players can describe the source game’s signature decision without being prompted.

Build the hall and one door first. Validate a second, mechanically distant door such as the harvester escort before authoring in batches of six. Reserve whole-party, low-end mobile and headset tests for each batch. The December dates are in-world release rules. The production estimate remains contingent on measured vertical-slice work and art capacity.

[Read all 24 mission treatments.](portals.md) · [Read the implementation and test plan.](engineering.md)
