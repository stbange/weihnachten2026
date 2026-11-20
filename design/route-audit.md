# Expedition route audit

The [design briefs](expedition-briefs.md) give each door its purpose, twist, feeling and decisions. This audit follows those decisions into the executable game. Run `python3 scripts/validate_routes.py build/coverage/replay` after the C tests. Add `--map 13 5` for the complete collision raster of a sector or `--write design/route-audit.md` to regenerate this book.

All 120 floor plans are authored. The five-letter-wide diagrams below label their rooms. The spine, branches and side loop columns are in the game's exact anchor notation. Letters joined in one word are connected by a passage. Spaces separate independent polylines. Full court radii, encounter placement, relief and chamber walls are in [floorplans.inc](../src/floorplans.inc), [fieldwork.inc](../src/fieldwork.inc), [districts.inc](../src/districts.inc) and [chambers.inc](../src/chambers.inc).

In each diagram S is the start, E the exit, * a required action room, V the hinted ward room, + a route station and . an optional landmark. Each coordinate is a four-metre tile. Graph cycles count independent loops in the authored normal corridors, before counting a commissioned route or the ward return. Graph dead ends name their contents. Typed actions below identify the placements on the critical route in encounter order.

The walk uses the production four-neighbour compass for each compulsory destination taken by the guided solo player. Tile length measures those centre-to-centre steps between actual player positions and targets. Rooms counts distinct authored rooms visited per sector, including rooms crossed en route. Optional share counts authored room anchors with no required interaction. It excludes individual roadside signals and includes both ends of the compulsory route in the denominator. Players can also shoot at range, cut diagonally or ride transport.

The graph search carries the ward key. It verifies that the party can reach the prize before opening its lock, walk to the exit with either key state, and reach every authored room again afterward. A second check reads the executable's collision tiles. The bots use movement and button inputs to verify both five-sector progress and actual extraction. Every field mark has a renewable fuel and movement reward. The room-gap measure counts room entries between a mark, a prize or an objective advance. Quiet seconds count the longest interval between these events. An opportunity on the route and a reward actually collected are measured separately.

Planned intensity is the designer's 1–10 judgement. Built intensity is a structural estimate from the playable score. An active beat starts at 3 and adds one per distinct staged action, one for a timing window, one for multiple required field signatures and one for the three-action finale. The shelter and safe extraction score 2. Seven is high. Actual timing and intervals between successes appear separately. Human excitement and first-clear time still need player observations.

## 01 · House Lights

Duke Nukem 3D · 1996

Make an ordinary cinema run by operating its fixtures. The switchboard in the stalls becomes a shooting gallery upstairs. Aim for the mischief of finding a working backstage door.

Shoot the passing frame or keep moving. Spend lift reaching the splice in the cutting room or save it for the booth. Return to the marquee through the staff line or approach the last screen from the stalls.

**Beats and graph**

Tram terminus (2) → Marquee foyer (5) → Orchestra pit (7) → Projection booth (4) → Empty stalls (2) → The final screening (9) → A punched ticket sets the first place (2).

Normal edges follow the spine, side passages and ward loop below. The first tool is the key to the chamber shutter. The mint return is a two-way shortcut earned in V. Sector exits are one-way after the required tools, transmission where present, and field practice. The final exit also requires three signature actions after shelter.

| Sector | Spine | Side passages | Ward loop | Route stations | Rooms / tiles | Optional rooms | Cycles | Dead ends and contents |
|---|---|---|---|---|---:|---:|---:|---|
| Tram terminus | `uvqlgbdiny` | `qlkfa bch mno` | `dei` | `qn` | 11 / 200 | 8/18 | 1 | a fuel signal, h fuel signal, m fuel signal, o fuel signal, u arrival landmark, y extraction |
| Marquee foyer | `acejonsrql` | `chm lgf rin ab` | `lkg` | `cn` | 12 / 270 | 7/17 | 2 | b tool / field reward, f fuel signal, m fuel signal |
| Orchestra pit | `kfbglqrsnm` | `lgm fhj qpu` | `qvr` | `lr` | 10 / 172 | 5/15 | 2 | j fuel signal, k arrival landmark, u fuel signal |
| Projection booth | `ytojidchmr` | `nsr ij gh` | `gbh` | `ih` | 12 / 344 | 5/14 | 1 | n tool / field reward, y arrival landmark |
| The final screening | `pkfaceoywm` | `kmo cqwm fv` | `fba` | `cm` | 12 / 302 | 2/13 | 4 | p arrival landmark, v fuel signal |

- **Tram terminus** · Start u. Landmark courts ugdin. Key Ticket punch at v → q opens the chamber shutter. Fight or aim rooms l Balcony ushers, g Projector changeover. Puzzle or traversal rooms Booth lift. Hinted reward e. Shortcut e ↔ u, opened at e. Lock and exit y.
- **Marquee foyer** · Start a. Landmark courts acjnr. Key Pleated usher at c → e opens the chamber shutter. Fight or aim rooms h Balcony ushers. Puzzle or traversal rooms b Stage door. Hinted reward k. Shortcut k ↔ a, opened at k. Lock and exit l.
- **Orchestra pit** · Start k. Landmark courts bgqsn. Key Pleated usher at f → b opens the chamber shutter. Fight or aim rooms g Projector changeover. Puzzle or traversal rooms l Stage door. Hinted reward v. Shortcut v ↔ k, opened at v. Lock and exit m.
- **Projection booth** · Start y. Landmark courts tjcmr. Key Booth lift at t → o opens the chamber shutter. Fight or aim rooms n Balcony ushers, i Projector changeover. Puzzle or traversal rooms Pleated usher. Hinted reward b. Shortcut b ↔ y, opened at b. Lock and exit r.
- **The final screening** · Start p. Landmark courts kcemw. Key Booth lift at k → f opens the chamber shutter. Fight or aim rooms c Balcony ushers, m Projector changeover. Puzzle or traversal rooms q Stage door. Hinted reward b. Shortcut b ↔ p, opened at b. Lock and exit m. Rest at Empty stalls.

**Layout and critical walk**

```text
    Sector 1          Sector 2          Sector 3          Sector 4          Sector 5
a. b* c. d* eV    aS b* c*    e*       b*                bV c* d*       a* bV c*    e*
f. g* h. i*       f. g. h* i. j*    f* g* h.    j.       g. h+ i* j*    f*
k. l* m. n* o.    kV lE m. n* o*    kS l* mE n*             m. n* o*    k*    mE    o*
   q*                q. r. s*       p. q* r* s*             rE s. t*    pS q*
uS v*       yE                      u. vV                         yS       v. w*    y*
```

- **Tram terminus** · v Ticket punch → q Ticket punch finish → l Balcony ushers → g Projector changeover → l Pleated usher → g Pleated usher finish → b Booth lift → d Booth lift finish → i Recover dispatch → n Answer dispatch → y Exit. Follow the tram rails to the ticket office. The service alley comes back behind the marquee.
- **Marquee foyer** · c Pleated usher → e Pleated usher finish → b Stage door → h Balcony ushers → j Ticket punch → o Ticket punch finish → n Booth lift → s Booth lift finish → l Exit. Two ticket aisles wrap the island kiosk. The staff corridor cuts behind both turnstiles.
- **Orchestra pit** · f Pleated usher → b Pleated usher finish → g Projector changeover → l Stage door → g Booth lift → l Booth lift finish → q Ticket punch → r Ticket punch finish → s Recover dispatch → n Answer dispatch → m Exit. The side aisles climb around the orchestra pit. Cross the stalls to reach the fly rig.
- **Projection booth** · t Booth lift → o Booth lift finish → n Balcony ushers → i Projector changeover → j Ticket punch → i Ticket punch finish → d Pleated usher → c Pleated usher finish → r Exit. Follow the film upstairs, around the reel cages and across the projection gallery.
- **The final screening** · k Booth lift → f Booth lift finish → a Pleated usher → c Pleated usher finish → e Ticket punch → o Ticket punch finish → y Recover dispatch → w Answer dispatch → (22,53) Empty stalls → q Stage door → c Balcony ushers → m Projector changeover → m Exit. The backlot wraps around the screen. Return along the illuminated centre aisle for the final frame.

**Content placement**

Marquee cutting room · Two staggered editing screens give a clear shot at a passing frame from either aisle.

```text
...:...
.XX:.!.
...:.X.
:::::::
.X.:...
.!.:XX.
...:...
```

X is solid cover, : is the approach inlay, ! is an amber warning lamp, and the middle tile holds the optional prize.

- Marquee bulb appears on travel lines. Players aim and pulse for lift fuel and a brief burst of speed while travelling.
- Changeover frame appears on travel lines. Players pulse on the bright beat for lift fuel and a brief burst of speed while travelling.
- Ticket slot appears on travel lines. Players press Use for lift fuel and a brief burst of speed while travelling.
- Balcony ushers is staged in a widened court. Fold the three moving paper ushers. The final sequence revisits this action after the shelter.
- Projector changeover is staged in a widened court. Pulse on the lit frame to change reels. The final sequence revisits this action after the shelter.
- Stage door is staged in a widened court. Use the brass latch to let the audience downstairs. The final sequence revisits this action after the shelter.
- Matinee splice sits between two entrances in the cover shown above. The orbiting amber lamp shows where contact will repel the player. Completing the indicated field skill awards a shared ward and opens the return line at the entrance.
- Empty stalls sits three tiles from the first finale anchor. It refills both seats and gives the party control over when to begin.

**Verification and playtest**

Planned intensity **2 → 5 → 7 → 4 → 2 → 9 → 2**. Built mechanical intensity **6 → 5 → 7 → 6 → 2 → 9 → 2**.

The guided path crossed **57 rooms and 1,288 tiles**. Optional room content was **35%** (27 of 77). The five sectors contained **10 independent normal-corridor cycles**. Every authored room has a reachable field reward within **9 tiles** of its centre. A reward is available in every room. The longest guided stretch outside the sensing radius of a field signal or compulsory destination was **1 tiles**.

| Player | Minutes | Marks answered | Wards found | Return rides | Longest quiet stretch | Most room entries between rewards | Climax begins |
|---|---:|---:|---:|---:|---:|---:|---:|
| Solo | 17.47 | 156 | 0 | 0 | 19.4 s | 3 | 93.2% |
| Split | 17.47 | 156 | 0 | 0 | 19.4 s | 3 | 93.2% |
| Rush | 21.48 | 8 | 0 | 0 | 107.6 s | 7 | 93.9% |
| Explore | 23.44 | 196 | 5 | 5 | 21.7 s | 3 | 95.0% |
| Cautious | 18.38 | 163 | 1 | 1 | 19.4 s | 3 | 93.6% |

**Rush.** Passing the signals left this player up to 108 seconds between successes. The field check catches a missed skill before extraction. Answering Marquee bulb in transit avoids a later detour.
**Explore.** Finding all five instances of Matinee splice and riding home added 6.0 minutes over the guided run. The mint home marker makes the destination legible before committing.
**Cautious.** The cautious player took the ward loop in the third sector, stunned nearby threats and finished in 18.38 minutes. Empty stalls gives the player a safe place to line up the final approach.
**Revision.** The old run let players pass long stretches with a single movement input. The side room added in this revision is Marquee cutting room. Two staggered editing screens give a clear shot at a passing frame from either aisle. This door now teaches Marquee bulb, develops Changeover frame and asks for all three field responses before the final demonstration. The five new ward rooms connect from both sides. The quickest of these observed policies still had to finish the same three final encounters.

## 02 · Weather for a Balloon

Magic Carpet 2: The Netherworlds · 1995

Read altitude to harvest embers and get a balloon home. Skimming the lee side pays where climbing into the wind previously paid. The pilot should feel the weight of looking after a slower companion.

Fly the thermal or skim the low trail. Break off for the ember vial or stay with the balloon. Board the return with the balloon close or let it follow the sheltered ground route.

**Beats and graph**

Wind arches (2) → Ember dunes (5) → Balloon mooring (7) → Basalt throat (4) → Sheltered mooring (2) → The candle vent (9) → The candle is still warm (2).

Normal edges follow the spine, side passages and ward loop below. The first tool is the key to the chamber shutter. The mint return is a two-way shortcut earned in V. Sector exits are one-way after the required tools, transmission where present, and field practice. The final exit also requires three signature actions after shelter.

| Sector | Spine | Side passages | Ward loop | Route stations | Rooms / tiles | Optional rooms | Cycles | Dead ends and contents |
|---|---|---|---|---|---:|---:|---:|---|
| Wind arches | `upkgcioswm` | `km mo wm` | `pqk` | `gm` | 9 / 301 | 2/11 | 3 | u arrival landmark |
| Ember dunes | `agmquwysoe` | `gmhio qrs` | `mnh` | `mw` | 8 / 184 | 6/14 | 3 | a arrival landmark, e extraction |
| Balloon mooring | `wrlfbdjnsm` | `lmn dhs` | `dej` | `rm` | 11 / 338 | 2/12 | 4 | w arrival landmark |
| Basalt throat | `eihgkpvwso` | `hmn lqw` | `idh` | `hg` | 9 / 218 | 6/15 | 1 | e arrival landmark, l fuel signal, n fuel signal, o extraction |
| The candle vent | `mgafkuvrne` | `mnh grv` | `nih` | `mn` | 9 / 284 | 3/12 | 3 | e extraction |

- **Wind arches** · Start u. Landmark courts kgios. Key Thermal sail at p opens the chamber shutter. Fight or aim rooms use the active tool sentry. Puzzle or traversal rooms g Ember on the wind, m Lee-side collector. Hinted reward q. Shortcut q ↔ u, opened at q. Lock and exit m.
- **Ember dunes** · Start a. Landmark courts gmqso. Key Earth rune at g → m opens the chamber shutter. Fight or aim rooms use the active tool sentry. Puzzle or traversal rooms m Lee-side collector, r Earth rune. Hinted reward n. Shortcut n ↔ a, opened at n. Lock and exit e.
- **Balloon mooring** · Start w. Landmark courts wlfjs. Key Ember balloon at r → l opens the chamber shutter. Fight or aim rooms use the active tool sentry. Puzzle or traversal rooms h Ember on the wind, r Earth rune. Hinted reward e. Shortcut e ↔ w, opened at e. Lock and exit m.
- **Basalt throat** · Start e. Landmark courts ihkpw. Key Earth rune at i → h opens the chamber shutter. Fight or aim rooms use the active tool sentry. Puzzle or traversal rooms h Lee-side collector, g Ember on the wind. Hinted reward d. Shortcut d ↔ e, opened at d. Lock and exit o.
- **The candle vent** · Start m. Landmark courts mfkrn. Key Thermal sail at g opens the chamber shutter. Fight or aim rooms use the active tool sentry. Puzzle or traversal rooms n Ember on the wind, m Lee-side collector, r Earth rune. Hinted reward i. Shortcut i ↔ m, opened at i. Lock and exit e. Rest at Sheltered mooring.

**Layout and critical walk**

```text
    Sector 1          Sector 2          Sector 3          Sector 4          Sector 5
      c*          aS          eE       b.    d* eV             dV eS    a.          eE
   g*    i*          g* h. i.       f*    h*    j*       g* h* i*       f* g* h. iV
k.    mE    o*          m* nV o*       l* mE n*       k* l. m. n. oE    k*    mS n*
p* qV    s*          q* r* s*             r* s*       p* q.    s*             r*
uS    w*          u.    w+    y.          wS             v. w*          u* v*
```

- **Wind arches** · p Thermal sail → g Ember on the wind → m Lee-side collector → g Ember balloon → c Ember balloon finish → i Earth rune → o Earth rune finish → s Recover dispatch → w Answer dispatch → m Exit. Three wind arches feed a broad thermal bowl. Skim the rim for the balloon or ride the centre lift.
- **Ember dunes** · g Earth rune → m Earth rune finish → m Lee-side collector → r Earth rune → q Thermal sail → s Recover dispatch → o Answer dispatch → e Exit. A crescent of dunes shelters the ember beds. Fly over the lee side to reach the mooring.
- **Balloon mooring** · r Ember balloon → l Ember balloon finish → h Ember on the wind → r Earth rune → f Thermal sail → d Earth rune → j Earth rune finish → n Recover dispatch → s Answer dispatch → m Exit. Outlying collectors circle the balloon anchor. The updraft is inside the ring.
- **Basalt throat** · i Earth rune → h Earth rune finish → h Lee-side collector → g Ember on the wind → g Ember balloon → k Ember balloon finish → p Thermal sail → w Recover dispatch → s Answer dispatch → o Exit. A bent basalt throat splits the sail route from the sheltered balloon passage.
- **The candle vent** · g Thermal sail → f Earth rune → k Earth rune finish → u Ember balloon → v Ember balloon finish → r Recover dispatch → n Answer dispatch → (53,41) Sheltered mooring → n Ember on the wind → m Lee-side collector → r Earth rune → e Exit. Bank the last ember in the caldera, then take a rising diagonal out of the vent.

**Content placement**

Balloon lee pocket · An open crescent lets the player dive into the lee and see the thermal on the way out.

```text
..:::..
.X:..!.
.X:....
.X::::.
..X..:.
.!XX.:.
...:::.
```

X is solid cover, : is the approach inlay, ! is an amber warning lamp, and the middle tile holds the optional prize.

- Thermal arch appears on travel lines. Players lift through the crown for lift fuel and a brief burst of speed while travelling.
- Lee ember appears on travel lines. Players cross at ground height for lift fuel and a brief burst of speed while travelling.
- Mooring rune appears on travel lines. Players press Use for lift fuel and a brief burst of speed while travelling.
- Ember on the wind is staged in a widened court. Fly through the high ember ring. The final sequence revisits this action after the shelter.
- Lee-side collector is staged in a widened court. Skim through the low collection hoop. The final sequence revisits this action after the shelter.
- Earth rune is staged in a widened court. Press Use to bank the embers and raise the mooring. The final sequence revisits this action after the shelter.
- Ember vial sits between two entrances in the cover shown above. The orbiting amber lamp shows where contact will repel the player. Completing the indicated field skill awards a shared ward and opens the return line at the entrance.
- Sheltered mooring sits three tiles from the first finale anchor. It refills both seats and gives the party control over when to begin.

**Verification and playtest**

Planned intensity **2 → 5 → 7 → 4 → 2 → 9 → 2**. Built mechanical intensity **5 → 5 → 6 → 5 → 2 → 8 → 2**.

The guided path crossed **46 rooms and 1,325 tiles**. Optional room content was **30%** (19 of 64). The five sectors contained **14 independent normal-corridor cycles**. Every authored room has a reachable field reward within **10 tiles** of its centre. A reward is available in every room. The longest guided stretch outside the sensing radius of a field signal or compulsory destination was **8 tiles**.

| Player | Minutes | Marks answered | Wards found | Return rides | Longest quiet stretch | Most room entries between rewards | Climax begins |
|---|---:|---:|---:|---:|---:|---:|---:|
| Solo | 18.29 | 123 | 0 | 0 | 21.0 s | 2 | 91.5% |
| Split | 18.29 | 123 | 0 | 0 | 21.0 s | 2 | 91.5% |
| Rush | 19.95 | 54 | 0 | 0 | 47.6 s | 2 | 91.9% |
| Explore | 22.15 | 142 | 5 | 5 | 25.3 s | 2 | 93.0% |
| Cautious | 19.64 | 127 | 1 | 1 | 25.3 s | 2 | 92.1% |

**Rush.** Passing the signals left this player up to 48 seconds between successes. The field check catches a missed skill before extraction. Answering Thermal arch in transit avoids a later detour.
**Explore.** Finding all five instances of Ember vial and riding home added 3.9 minutes over the guided run. The mint home marker makes the destination legible before committing.
**Cautious.** The cautious player took the ward loop in the third sector, stunned nearby threats and finished in 19.64 minutes. Sheltered mooring gives the player a safe place to line up the final approach.
**Revision.** The old run let players pass long stretches with a single movement input. The side room added in this revision is Balloon lee pocket. An open crescent lets the player dive into the lee and see the thermal on the way out. This door now teaches Thermal arch, develops Lee ember and asks for all three field responses before the final demonstration. The five new ward rooms connect from both sides. The quickest of these observed policies still had to finish the same three final encounters.

## 03 · The Glass Convoy

Command & Conquer · 1995 · Tiberian Dawn

Make one complete survey, harvest and delivery circuit. The ore road that made the outbound run quick has to serve a loaded harvester on the return. Aim for a working supply line.

Mark the survey posts or commission the service road. Work the assay room or go straight to the scale. Lead the harvester along the return line or handle the relay at the weighbridge on foot.

**Beats and graph**

Survey camp (2) → Bottle-glass seam (5) → Refinery road (7) → Loading bay (4) → Survey cab (2) → The ice fence (9) → A brass loading token carries the same punch as the cinema ticket (2).

Normal edges follow the spine, side passages and ward loop below. The first tool is the key to the chamber shutter. The mint return is a two-way shortcut earned in V. Sector exits are one-way after the required tools, transmission where present, and field practice. The final exit also requires three signature actions after shelter.

| Sector | Spine | Side passages | Ward loop | Route stations | Rooms / tiles | Optional rooms | Cycles | Dead ends and contents |
|---|---|---|---|---|---:|---:|---:|---|
| Survey camp | `upkfachmoy` | `klm cdejo` | `cbh` | `km` | 10 / 263 | 5/15 | 3 | u arrival landmark, y extraction |
| Bottle-glass seam | `ejotywqlfa` | `onml wvu` | `mhl` | `ol` | 11 / 263 | 4/15 | 2 | a extraction, e arrival landmark, u fuel signal |
| Refinery road | `klgbdinsxw` | `lmn nsrqp` | `ihn` | `ln` | 11 / 261 | 6/15 | 2 | k arrival landmark, p fuel signal, w extraction |
| Loading bay | `afkuwrmhde` | `klm ghib` | `kpl` | `km` | 12 / 263 | 5/15 | 2 | a arrival landmark, b fuel signal, e extraction, g fuel signal |
| The ice fence | `vqlgcdjoty` | `lmn no qrst` | `mhn` | `lo` | 12 / 311 | 4/15 | 3 | v arrival landmark, y extraction |

- **Survey camp** · Start u. Landmark courts ukahoy. Key Route stakes at p → k opens the chamber shutter. Fight or aim rooms l Glass seam. Puzzle or traversal rooms m Construction sled. Hinted reward b. Shortcut b ↔ u, opened at b. Lock and exit y.
- **Bottle-glass seam** · Start e. Landmark courts eowqf. Key Glass harvester at j → o opens the chamber shutter. Fight or aim rooms n Glass seam, m Power allocation. Puzzle or traversal rooms Substation. Hinted reward h. Shortcut h ↔ e, opened at h. Lock and exit a.
- **Refinery road** · Start k. Landmark courts kginx. Key Glass harvester at l → g opens the chamber shutter. Fight or aim rooms n Power allocation. Puzzle or traversal rooms l Construction sled. Hinted reward h. Shortcut h ↔ k, opened at h. Lock and exit w.
- **Loading bay** · Start a. Landmark courts fkurd. Key Substation at f opens the chamber shutter. Fight or aim rooms l Power allocation. Puzzle or traversal rooms m Construction sled. Hinted reward p. Shortcut p ↔ a, opened at p. Lock and exit e.
- **The ice fence** · Start v. Landmark courts qgdot. Key Substation at q opens the chamber shutter. Fight or aim rooms n Glass seam, l Power allocation. Puzzle or traversal rooms o Construction sled. Hinted reward h. Shortcut h ↔ v, opened at h. Lock and exit y. Rest at Survey cab.

**Layout and critical walk**

```text
    Sector 1          Sector 2          Sector 3          Sector 4          Sector 5
a. bV c* d. e.    aE          eS       b*    d.       aS b.    d* eE          c* d*
f*    h*    j.    f*    hV    j*       g* hV i*       f* g. h* i.          g* hV    j*
k* l* m*    o*       l* m* n* o*    kS l* m. n*       k+ l* m*             l* m. n* o*
p*                   q.       t*    p. q. r. s*       pV    r*             q* r. s. t*
uS          yE    u. v. w*    y*          wE x*       u*    w*             vS       yE
```

- **Survey camp** · p Route stakes → k Route stakes finish → l Glass seam → m Construction sled → f Substation → c Glass harvester → h Glass harvester finish → m Recover dispatch → o Answer dispatch → y Exit. Survey the western boundary before sending the harvester over the service road to the east.
- **Bottle-glass seam** · j Glass harvester → o Glass harvester finish → n Glass seam → m Power allocation → t Route stakes → y Route stakes finish → w Substation → l Recover dispatch → f Answer dispatch → a Exit. The glass seam hooks around the field. Empty vehicles can cut across the inner belt.
- **Refinery road** · l Glass harvester → g Glass harvester finish → l Construction sled → n Power allocation → b Substation → i Route stakes → n Route stakes finish → s Recover dispatch → x Answer dispatch → w Exit. Stake the refinery bypass. The returning harvester needs the scale at the foot of the silos.
- **Loading bay** · f Substation → l Power allocation → m Construction sled → u Route stakes → w Route stakes finish → r Glass harvester → m Glass harvester finish → h Recover dispatch → d Answer dispatch → e Exit. Two loading aprons share a crane. Power the cross-yard lane before bringing a vehicle through.
- **The ice fence** · q Substation → g Glass harvester → c Glass harvester finish → d Route stakes → j Route stakes finish → o Recover dispatch → t Answer dispatch → (56,43) Survey cab → n Glass seam → l Power allocation → o Construction sled → y Exit. The ice fence separates the convoy from the yard. Its power feed runs around the far flank.

**Content placement**

Assay weighbridge · Parallel loading cheeks leave a straight haul lane through the sample scale.

```text
..:::..
.!:.X..
.X:....
.X::::.
....:X.
..X.:!.
..:::..
```

X is solid cover, : is the approach inlay, ! is an amber warning lamp, and the middle tile holds the optional prize.

- Survey stake appears on travel lines. Players press Use for lift fuel and a brief burst of speed while travelling.
- Ore glint appears on travel lines. Players aim and pulse for lift fuel and a brief burst of speed while travelling.
- Haul lane appears on travel lines. Players cross at ground height for lift fuel and a brief burst of speed while travelling.
- Power allocation is staged in a widened court. Tune the requested load and pulse to reroute the grid. The final sequence revisits this action after the shelter.
- Construction sled is staged in a widened court. Use to deploy the sled at the slab. The final sequence revisits this action after the shelter.
- Glass seam is staged in a widened court. Hold the beam on the seam until the ore breaks free. The final sequence revisits this action after the shelter.
- Glass assay sits between two entrances in the cover shown above. The orbiting amber lamp shows where contact will repel the player. Completing the indicated field skill awards a shared ward and opens the return line at the entrance.
- Survey cab sits three tiles from the first finale anchor. It refills both seats and gives the party control over when to begin.

**Verification and playtest**

Planned intensity **2 → 5 → 7 → 4 → 2 → 9 → 2**. Built mechanical intensity **5 → 5 → 6 → 5 → 2 → 8 → 2**.

The guided path crossed **56 rooms and 1,361 tiles**. Optional room content was **32%** (24 of 75). The five sectors contained **12 independent normal-corridor cycles**. Every authored room has a reachable field reward within **7 tiles** of its centre. A reward is available in every room. The longest guided stretch outside the sensing radius of a field signal or compulsory destination was **1 tiles**.

| Player | Minutes | Marks answered | Wards found | Return rides | Longest quiet stretch | Most room entries between rewards | Climax begins |
|---|---:|---:|---:|---:|---:|---:|---:|
| Solo | 18.27 | 192 | 1 | 0 | 16.6 s | 2 | 91.6% |
| Split | 18.28 | 192 | 1 | 0 | 16.6 s | 2 | 91.6% |
| Rush | 19.89 | 75 | 0 | 0 | 42.2 s | 2 | 91.7% |
| Explore | 22.98 | 227 | 5 | 5 | 19.5 s | 2 | 93.3% |
| Cautious | 19.21 | 199 | 2 | 1 | 19.5 s | 2 | 92.0% |

**Rush.** Passing the signals left this player up to 42 seconds between successes. The field check catches a missed skill before extraction. Answering Survey stake in transit avoids a later detour.
**Explore.** Finding all five instances of Glass assay and riding home added 4.7 minutes over the guided run. The mint home marker makes the destination legible before committing.
**Cautious.** The cautious player took the ward loop in the third sector, stunned nearby threats and finished in 19.21 minutes. Survey cab gives the player a safe place to line up the final approach.
**Revision.** The old run let players pass long stretches with a single movement input. The side room added in this revision is Assay weighbridge. Parallel loading cheeks leave a straight haul lane through the sample scale. This door now teaches Survey stake, develops Ore glint and asks for all three field responses before the final demonstration. The five new ward rooms connect from both sides. The quickest of these observed policies still had to finish the same three final encounters.

## 04 · The Bell Under Ice

Tomb Raider · 1996 · original

Learn the cistern by recognising a ledge from above and below. Releasing the counterweight turns the route you saw across the water into a crossing. Aim for the moment a ruin makes sense.

Read the landing from the shelf or commit to the jump. Spend lift at the bellmaker's rubbing or carry it to the grapple. Re-enter from the lip or follow the basin floor.

**Beats and graph**

Dry basin (2) → Weight gallery (5) → Overflow channel (7) → Bell vault (4) → Dry landing (2) → The cistern lip (9) → The clapper has a loop of LAN cable where a rope should be (2).

Normal edges follow the spine, side passages and ward loop below. The first tool is the key to the chamber shutter. The mint return is a two-way shortcut earned in V. Sector exits are one-way after the required tools, transmission where present, and field practice. The final exit also requires three signature actions after shelter.

| Sector | Spine | Side passages | Ward loop | Route stations | Rooms / tiles | Optional rooms | Cycles | Dead ends and contents |
|---|---|---|---|---|---:|---:|---:|---|
| Dry basin | `abchmrwvpk` | `mno pql` | `bgc` | `hr` | 9 / 227 | 6/15 | 1 | a arrival landmark, k extraction, l fuel signal, o fuel signal |
| Weight gallery | `yxwrmlkfbc` | `rqv ghc` | `rsm` | `ml` | 10 / 225 | 5/15 | 1 | g fuel signal, v fuel signal, y arrival landmark |
| Overflow channel | `fglqvwxsnj` | `lmnr hi gh` | `stn` | `ln` | 11 / 229 | 7/15 | 2 | f arrival landmark, i fuel signal, j extraction, r fuel signal |
| Bell vault | `chgfkuwyto` | `hms klmno` | `him` | `hm` | 11 / 259 | 5/15 | 3 | c arrival landmark, s fuel signal |
| The cistern lip | `mnidbfpvwc` | `mrg wh` | `baf` | `mi` | 14 / 389 | 3/14 | 1 | c extraction, g tool / field reward, h fuel signal |

- **Dry basin** · Start a. Landmark courts chmwp. Key Ledge grapple at b → c opens the chamber shutter. Fight or aim rooms use the active tool sentry. Puzzle or traversal rooms h Cistern ledge, r Balance cradle. Hinted reward g. Shortcut g ↔ a, opened at g. Lock and exit k.
- **Weight gallery** · Start y. Landmark courts xrmkb. Key Counterweight at x → w opens the chamber shutter. Fight or aim rooms use the active tool sentry. Puzzle or traversal rooms q Balance cradle, m Cistern ledge. Hinted reward s. Shortcut s ↔ y, opened at s. Lock and exit c.
- **Overflow channel** · Start f. Landmark courts glwsn. Key Clockwork bat at g opens the chamber shutter. Fight or aim rooms m Bats in the vault. Puzzle or traversal rooms n Cistern ledge. Hinted reward t. Shortcut t ↔ f, opened at t. Lock and exit j.
- **Bell vault** · Start c. Landmark courts hkuwt. Key Counterweight at h → g opens the chamber shutter. Fight or aim rooms m Bats in the vault. Puzzle or traversal rooms h Balance cradle. Hinted reward i. Shortcut i ↔ c, opened at i. Lock and exit o.
- **The cistern lip** · Start m. Landmark courts mnfpw. Key Clockwork bat at n opens the chamber shutter. Fight or aim rooms m Bats in the vault. Puzzle or traversal rooms i Cistern ledge, g Balance cradle. Hinted reward a. Shortcut a ↔ m, opened at a. Lock and exit c. Rest at Dry landing.

**Layout and critical walk**

```text
    Sector 1          Sector 2          Sector 3          Sector 4          Sector 5
aS b* c*             b* cE                                  cS          aV b* cE d*
   gV h*          f* g. h.          fS g* h. i. jE    f* g* h* iV       f* g* h. i*
kE l. m* n. o.    k. l* m*             l+ m* n*       k. l. m* n. oE          mS n*
p* q. r*             q* r* sV          q* r. s* tV             s. t*    p*    r.
   v* w.             v. w* x* yS       v* w. x.       u*    w*    y*       v* w*
```

- **Dry basin** · b Ledge grapple → c Ledge grapple finish → h Cistern ledge → r Balance cradle → h Counterweight → m Counterweight finish → r Clockwork bat → v Recover dispatch → p Answer dispatch → k Exit. Descend the stepped lip of the dry basin. A high ledge looks back across the entire reservoir.
- **Weight gallery** · x Counterweight → w Counterweight finish → q Balance cradle → m Cistern ledge → r Ledge grapple → m Ledge grapple finish → l Clockwork bat → f Recover dispatch → b Answer dispatch → c Exit. Counterweight tracks run at right angles to the gallery. Carry the weight back across its own channel.
- **Overflow channel** · g Clockwork bat → m Bats in the vault → n Cistern ledge → q Ledge grapple → v Ledge grapple finish → s Recover dispatch → n Answer dispatch → j Exit. An overflow trench bends underneath the bell gallery. Grapple from its last dry shelf.
- **Bell vault** · h Counterweight → g Counterweight finish → h Balance cradle → m Bats in the vault → f Clockwork bat → u Ledge grapple → w Ledge grapple finish → y Recover dispatch → t Answer dispatch → o Exit. A square ambulatory encloses the bell. Its raised latch looks down on the drained vault.
- **The cistern lip** · n Clockwork bat → d Ledge grapple → b Ledge grapple finish → f Counterweight → p Counterweight finish → v Recover dispatch → w Answer dispatch → (40,43) Dry landing → m Bats in the vault → i Cistern ledge → g Balance cradle → c Exit. Climb out along alternating lips and recover the clapper above the entrance.

**Content placement**

Bellmaker's shelf · Two L-shaped plinths frame the rubbing from below and give the grapple a visible landing.

```text
....:..
.XX.:!.
.X..:..
.:::::.
..:..X.
.!:.XX.
..:....
```

X is solid cover, : is the approach inlay, ! is an amber warning lamp, and the middle tile holds the optional prize.

- Stone brow appears on travel lines. Players lift through the crown for lift fuel and a brief burst of speed while travelling.
- Weight cradle appears on travel lines. Players press Use for lift fuel and a brief burst of speed while travelling.
- Clockwork bat appears on travel lines. Players aim and pulse for lift fuel and a brief burst of speed while travelling.
- Cistern ledge is staged in a widened court. Catch the high stone brow with Lift. The final sequence revisits this action after the shelter.
- Balance cradle is staged in a widened court. Set the counterweight in its cradle with Use. The final sequence revisits this action after the shelter.
- Bats in the vault is staged in a widened court. Pulse each of the three moving clockwork bats. The final sequence revisits this action after the shelter.
- Bellmaker's rubbing sits between two entrances in the cover shown above. The orbiting amber lamp shows where contact will repel the player. Completing the indicated field skill awards a shared ward and opens the return line at the entrance.
- Dry landing sits three tiles from the first finale anchor. It refills both seats and gives the party control over when to begin.

**Verification and playtest**

Planned intensity **2 → 5 → 7 → 4 → 2 → 9 → 2**. Built mechanical intensity **5 → 5 → 6 → 5 → 2 → 8 → 2**.

The guided path crossed **55 rooms and 1,329 tiles**. Optional room content was **35%** (26 of 74). The five sectors contained **8 independent normal-corridor cycles**. Every authored room has a reachable field reward within **6 tiles** of its centre. A reward is available in every room. The longest guided stretch outside the sensing radius of a field signal or compulsory destination was **2 tiles**.

| Player | Minutes | Marks answered | Wards found | Return rides | Longest quiet stretch | Most room entries between rewards | Climax begins |
|---|---:|---:|---:|---:|---:|---:|---:|
| Solo | 17.99 | 194 | 1 | 0 | 17.5 s | 2 | 87.7% |
| Split | 17.99 | 194 | 1 | 0 | 17.5 s | 2 | 87.7% |
| Rush | 21.23 | 9 | 0 | 0 | 57.7 s | 4 | 88.8% |
| Explore | 21.84 | 231 | 5 | 5 | 17.5 s | 2 | 89.9% |
| Cautious | 19.25 | 205 | 2 | 1 | 17.5 s | 2 | 88.5% |

**Rush.** Passing the signals left this player up to 58 seconds between successes. The field check catches a missed skill before extraction. Answering Stone brow in transit avoids a later detour.
**Explore.** Finding all five instances of Bellmaker's rubbing and riding home added 3.9 minutes over the guided run. The mint home marker makes the destination legible before committing.
**Cautious.** The cautious player took the ward loop in the third sector, stunned nearby threats and finished in 19.25 minutes. Dry landing gives the player a safe place to line up the final approach.
**Revision.** The old run let players pass long stretches with a single movement input. The side room added in this revision is Bellmaker's shelf. Two L-shaped plinths frame the rubbing from below and give the grapple a visible landing. This door now teaches Stone brow, develops Weight cradle and asks for all three field responses before the final demonstration. The five new ward rooms connect from both sides. The quickest of these observed policies still had to finish the same three final encounters.

## 05 · The Candle Fleet

Warcraft II: Tides of Darkness · 1995

Keep a little port supplied by timber, a scout and a loaded tanker. The berth used for scouting also carries a following tanker. Aim for the bustle of a port waking up.

Sound the pilot lights or use the ordinary quay. Detour for the chart or stay with the tanker. Board together at the sail berth or escort along the shoreline.

**Beats and graph**

Frozen sawmill (2) → Tanker quay (5) → Sounding bells (7) → Lantern harbour (4) → Harbourmaster's loft (2) → Breakwater (9) → A little wooden ship gives the first table its centrepiece (2).

Normal edges follow the spine, side passages and ward loop below. The first tool is the key to the chamber shutter. The mint return is a two-way shortcut earned in V. Sector exits are one-way after the required tools, transmission where present, and field practice. The final exit also requires three signature actions after shelter.

| Sector | Spine | Side passages | Ward loop | Route stations | Rooms / tiles | Optional rooms | Cycles | Dead ends and contents |
|---|---|---|---|---|---:|---:|---:|---|
| Frozen sawmill | `afklqvwrno` | `klm hmr` | `fgk` | `lr` | 9 / 196 | 5/13 | 2 | a arrival landmark, h fuel signal, o extraction |
| Tanker quay | `uvqlgbdins` | `qrm ghij` | `gfb` | `qn` | 11 / 254 | 5/15 | 2 | j fuel signal, m fuel signal, s extraction, u arrival landmark |
| Sounding bells | `ejihcbglqp` | `imr lmn` | `lkq` | `il` | 11 / 210 | 3/14 | 2 | e arrival landmark, n fuel signal, p extraction, r fuel signal |
| Lantern harbour | `kpuwytojdc` | `kmno wrc` | `mhn` | `ko` | 13 / 284 | 3/14 | 3 | None |
| Breakwater | `wvpkfbciny` | `kql hmn` | `mrn` | `pn` | 12 / 474 | 4/15 | 1 | h tool / field reward, l fuel signal, w arrival landmark, y extraction |

- **Frozen sawmill** · Start a. Landmark courts aklwn. Key Repair pennant at f → k opens the chamber shutter. Fight or aim rooms r Harbour pilot. Puzzle or traversal rooms l Repair gang. Hinted reward g. Shortcut g ↔ a, opened at g. Lock and exit o.
- **Tanker quay** · Start u. Landmark courts vlgis. Key Oil tanker at v → q opens the chamber shutter. Fight or aim rooms n Harbour pilot. Puzzle or traversal rooms q Scout sounding. Hinted reward f. Shortcut f ↔ u, opened at f. Lock and exit s.
- **Sounding bells** · Start e. Landmark courts jchlp. Key Scout pennant at j → i opens the chamber shutter. Fight or aim rooms l Harbour pilot. Puzzle or traversal rooms m Scout sounding. Hinted reward k. Shortcut k ↔ e, opened at k. Lock and exit p.
- **Lantern harbour** · Start k. Landmark courts kuwod. Key Repair pennant at p → u opens the chamber shutter. Fight or aim rooms use the active tool sentry. Puzzle or traversal rooms m Scout sounding, o Repair gang. Hinted reward h. Shortcut h ↔ k, opened at h. Lock and exit c.
- **Breakwater** · Start w. Landmark courts vpkci. Key Oil tanker at v → p opens the chamber shutter. Fight or aim rooms h Harbour pilot. Puzzle or traversal rooms p Repair gang, n Scout sounding. Hinted reward r. Shortcut r ↔ w, opened at r. Lock and exit y. Rest at Harbourmaster's loft.

**Layout and critical walk**

```text
    Sector 1          Sector 2          Sector 3          Sector 4          Sector 5
aS                   b*    d*          b* c*    eS          cE d*          b* c*
f* gV h.          fV g* h. i* j.       g* h* i* j*          hV    j*    f*    h* i*
k* l* m. n* oE       l* m. n*       kV l* m* n.       kS    m* n. o*    k* l. m. n*
   q* r*             q* r. sE       pE q* r.          p*    r.    t*    p* q. rV
   v. w.          uS v*                               u*    w*    y*       v* wS    yE
```

- **Frozen sawmill** · f Repair pennant → k Repair pennant finish → l Repair gang → r Harbour pilot → l Scout pennant → q Scout pennant finish → r Recover dispatch → n Answer dispatch → o Exit. The timber road ends at the slipway. Send the repair pennant along the jetty.
- **Tanker quay** · v Oil tanker → q Oil tanker finish → q Scout sounding → n Harbour pilot → l Repair pennant → g Repair pennant finish → b Scout pennant → d Scout pennant finish → i Recover dispatch → n Answer dispatch → s Exit. One long tanker pier faces a parallel oil platform. Cross at the marked ferry berth.
- **Sounding bells** · j Scout pennant → i Scout pennant finish → l Harbour pilot → m Scout sounding → h Oil tanker → c Oil tanker finish → b Repair pennant → g Repair pennant finish → l Recover dispatch → q Answer dispatch → p Exit. Thread the islands by their sounding bells. The scout landing lies behind the last headland.
- **Lantern harbour** · p Repair pennant → u Repair pennant finish → m Scout sounding → o Repair gang → w Oil tanker → y Oil tanker finish → t Scout pennant → o Scout pennant finish → j Recover dispatch → d Answer dispatch → c Exit. Three docks radiate into a horseshoe harbour. The inside channel brings tankers home.
- **Breakwater** · v Oil tanker → p Oil tanker finish → k Scout pennant → f Scout pennant finish → b Repair pennant → c Repair pennant finish → i Recover dispatch → n Answer dispatch → (8,53) Harbourmaster's loft → p Repair gang → n Scout sounding → h Harbour pilot → y Exit. Follow the breakwater out to its lantern, then bring the fleet home along the sheltered arm.

**Content placement**

Pilot's chart house · Offset piers make a dogleg around the chart table with a view back toward the berth.

```text
...::..
.!X.:..
..X.:..
..::::.
..:.XX.
..:.!..
.::....
```

X is solid cover, : is the approach inlay, ! is an amber warning lamp, and the middle tile holds the optional prize.

- Pier bell appears on travel lines. Players press Use for lift fuel and a brief burst of speed while travelling.
- Pilot light appears on travel lines. Players pulse on the bright beat for lift fuel and a brief burst of speed while travelling.
- Tanker wake appears on travel lines. Players cross at ground height for lift fuel and a brief burst of speed while travelling.
- Repair gang is staged in a widened court. Plant the repair pennant with Use. The final sequence revisits this action after the shelter.
- Scout sounding is staged in a widened court. Cross the sounding buoy at water level. The final sequence revisits this action after the shelter.
- Harbour pilot is staged in a widened court. Tune the signal lanterns and pulse the pilot code. The final sequence revisits this action after the shelter.
- Pilot's chart sits between two entrances in the cover shown above. The orbiting amber lamp shows where contact will repel the player. Completing the indicated field skill awards a shared ward and opens the return line at the entrance.
- Harbourmaster's loft sits three tiles from the first finale anchor. It refills both seats and gives the party control over when to begin.

**Verification and playtest**

Planned intensity **2 → 5 → 7 → 4 → 2 → 9 → 2**. Built mechanical intensity **5 → 5 → 6 → 5 → 2 → 8 → 2**.

The guided path crossed **56 rooms and 1,418 tiles**. Optional room content was **28%** (20 of 71). The five sectors contained **10 independent normal-corridor cycles**. Every authored room has a reachable field reward within **7 tiles** of its centre. A reward is available in every room. The longest guided stretch outside the sensing radius of a field signal or compulsory destination was **2 tiles**.

| Player | Minutes | Marks answered | Wards found | Return rides | Longest quiet stretch | Most room entries between rewards | Climax begins |
|---|---:|---:|---:|---:|---:|---:|---:|
| Solo | 19.16 | 159 | 0 | 0 | 28.7 s | 2 | 85.0% |
| Split | 19.16 | 159 | 0 | 0 | 28.8 s | 2 | 85.0% |
| Rush | 23.66 | 68 | 0 | 0 | 49.0 s | 2 | 87.4% |
| Explore | 24.63 | 194 | 5 | 5 | 34.8 s | 3 | 88.4% |
| Cautious | 20.34 | 166 | 1 | 1 | 28.7 s | 2 | 85.9% |

**Rush.** Passing the signals left this player up to 49 seconds between successes. The field check catches a missed skill before extraction. Answering Pier bell in transit avoids a later detour.
**Explore.** Finding all five instances of Pilot's chart and riding home added 5.5 minutes over the guided run. The mint home marker makes the destination legible before committing.
**Cautious.** The cautious player took the ward loop in the third sector, stunned nearby threats and finished in 20.34 minutes. Harbourmaster's loft gives the player a safe place to line up the final approach.
**Revision.** The old run let players pass long stretches with a single movement input. The side room added in this revision is Pilot's chart house. Offset piers make a dogleg around the chart table with a view back toward the berth. This door now teaches Pier bell, develops Pilot light and asks for all three field responses before the final demonstration. The five new ward rooms connect from both sides. The quickest of these observed policies still had to finish the same three final encounters.

## 06 · A Giant Carries Supper

MechWarrior · 1989 · original PC game

Choose where a heavy walker can fire, cool and carry its load. The useful cooling stops sit off the companion's route. Aim for deliberate control of a heavy machine.

Line up a range shutter or head for coolant. Reclaim the heat-sink vane or keep pace with the walker. Use the return line with company or finish across the salt road.

**Beats and graph**

Salvage shed (2) → Salt road (5) → Cooling vents (7) → Windcut bridge (4) → Cold manifold (2) → Mess yard (9) → A folding chair clicks open and six recovered settings reveal footage from a party shot by an unseen guest (2).

Normal edges follow the spine, side passages and ward loop below. The first tool is the key to the chamber shutter. The mint return is a two-way shortcut earned in V. Sector exits are one-way after the required tools, transmission where present, and field practice. The final exit also requires three signature actions after shelter.

| Sector | Spine | Side passages | Ward loop | Route stations | Rooms / tiles | Optional rooms | Cycles | Dead ends and contents |
|---|---|---|---|---|---:|---:|---:|---|
| Salvage shed | `pkfghmnoty` | `klm tuv` | `fag` | `gn` | 10 / 251 | 6/14 | 2 | p arrival landmark, v fuel signal, y extraction |
| Salt road | `aglqwxsnie` | `lmn hmr` | `noi` | `qn` | 10 / 310 | 5/14 | 2 | a arrival landmark, e extraction, h fuel signal, r fuel signal |
| Cooling vents | `onsxwvqlgb` | `nmh src` | `stx` | `sl` | 11 / 304 | 4/15 | 1 | b extraction, c fuel signal, h fuel signal, o arrival landmark |
| Windcut bridge | `upqrmhijoy` | `qgl rst` | `hci` | `ri` | 10 / 270 | 8/15 | 1 | l fuel signal, t fuel signal, u arrival landmark, y extraction |
| Mess yard | `ejonmlqvwr` | `imr lfk` | `otn` | `nr` | 10 / 289 | 4/14 | 2 | e arrival landmark, i fuel signal, k fuel signal |

- **Salvage shed** · Start p. Landmark courts kfhmoy. Key Chair crate at k → f opens the chamber shutter. Fight or aim rooms h Walker heat beam, m Coolant stop. Puzzle or traversal rooms Companion walker. Hinted reward a. Shortcut a ↔ p, opened at a. Lock and exit y.
- **Salt road** · Start a. Landmark courts alwxne. Key Heat beam at g → l opens the chamber shutter. Fight or aim rooms l Range shutters, n Coolant stop. Puzzle or traversal rooms Chair crate. Hinted reward o. Shortcut o ↔ a, opened at o. Lock and exit e.
- **Cooling vents** · Start o. Landmark courts onswlg. Key Heat beam at n → s opens the chamber shutter. Fight or aim rooms s Coolant stop, l Walker heat beam, m Range shutters. Puzzle or traversal rooms Companion walker. Hinted reward t. Shortcut t ↔ o, opened at t. Lock and exit b.
- **Windcut bridge** · Start u. Landmark courts pqrmjo. Key Companion walker at p → q opens the chamber shutter. Fight or aim rooms r Range shutters, i Walker heat beam. Puzzle or traversal rooms Heat beam. Hinted reward c. Shortcut c ↔ u, opened at c. Lock and exit y.
- **Mess yard** · Start e. Landmark courts eonlvr. Key Chair crate at j → o opens the chamber shutter. Fight or aim rooms n Walker heat beam, r Coolant stop, m Range shutters. Puzzle or traversal rooms Heat beam. Hinted reward t. Shortcut t ↔ e, opened at t. Lock and exit r. Rest at Cold manifold.

**Layout and critical walk**

```text
    Sector 1          Sector 2          Sector 3          Sector 4          Sector 5
aV                aS          eE       bE c.                cV                      eS
f* g* h*             g* h. i.          g* h.             g. h. i* j.    f.       i. j*
k* l. m* n* o.       l* m. n* oV       l* m* n* oS       l. m*    o.    k. l* m* n* o*
pS          t.       q* r. s*          q* r. s* tV    p* q* r* s. t.       q* rE    tV
u. v.       yE          w* x*          v* w* x*       uS          yE       v* w*
```

- **Salvage shed** · k Chair crate → f Chair crate finish → h Walker heat beam → m Coolant stop → g Heat beam → h Heat beam finish → m Companion walker → n Companion walker finish → y Exit. Salvage bays open onto a testing straight. Load the chair crate before crossing the apron.
- **Salt road** · g Heat beam → l Heat beam finish → l Range shutters → n Coolant stop → q Companion walker → w Companion walker finish → x Chair crate → s Chair crate finish → e Exit. The road skirts a salt sink. Cooling pads sit off the exposed line between the ridges.
- **Cooling vents** · n Heat beam → s Heat beam finish → s Coolant stop → l Walker heat beam → m Range shutters → x Chair crate → w Chair crate finish → v Companion walker → q Companion walker finish → l Recover dispatch → g Answer dispatch → b Exit. Walk the zigzag cooling manifold. The companion can hold the central apron during the beam cycle.
- **Windcut bridge** · p Companion walker → q Companion walker finish → r Range shutters → i Walker heat beam → r Heat beam → m Heat beam finish → y Exit. Approach the trestle through windbreaks. The sheltered spur feeds back onto the bridge.
- **Mess yard** · j Chair crate → o Chair crate finish → n Companion walker → m Companion walker finish → l Heat beam → q Heat beam finish → v Recover dispatch → w Answer dispatch → (65,49) Cold manifold → n Walker heat beam → r Coolant stop → m Range shutters → r Exit. The mess yard has a freight entrance and a narrow canteen wing. Deliver the chairs at the hearth.

**Content placement**

Salt-road machine bay · Broken rows of cooling fins offer openings from different firing angles.

```text
..:....
.X:.X!.
..:....
.:::::.
....:..
.!X.:X.
....:..
```

X is solid cover, : is the approach inlay, ! is an amber warning lamp, and the middle tile holds the optional prize.

- Salvage plate appears on travel lines. Players charge and release on the plate for lift fuel and a brief burst of speed while travelling.
- Heat shutter appears on travel lines. Players pulse on the bright beat for lift fuel and a brief burst of speed while travelling.
- Range vane appears on travel lines. Players aim and pulse for lift fuel and a brief burst of speed while travelling.
- Walker heat beam is staged in a widened court. Hold the torso beam steady on the frozen rack. The final sequence revisits this action after the shelter.
- Coolant stop is staged in a widened court. Use the vent to flush the hot manifold. The final sequence revisits this action after the shelter.
- Range shutters is staged in a widened court. Traverse the three moving range shutters and pulse each one. The final sequence revisits this action after the shelter.
- Heat-sink vane sits between two entrances in the cover shown above. The orbiting amber lamp shows where contact will repel the player. Completing the indicated field skill awards a shared ward and opens the return line at the entrance.
- Cold manifold sits three tiles from the first finale anchor. It refills both seats and gives the party control over when to begin.

**Verification and playtest**

Planned intensity **2 → 5 → 7 → 4 → 2 → 9 → 2**. Built mechanical intensity **5 → 5 → 7 → 5 → 2 → 8 → 2**.

The guided path crossed **51 rooms and 1,424 tiles**. Optional room content was **38%** (27 of 72). The five sectors contained **8 independent normal-corridor cycles**. Every authored room has a reachable field reward within **8 tiles** of its centre. A reward is available in every room. The longest guided stretch outside the sensing radius of a field signal or compulsory destination was **6 tiles**.

| Player | Minutes | Marks answered | Wards found | Return rides | Longest quiet stretch | Most room entries between rewards | Climax begins |
|---|---:|---:|---:|---:|---:|---:|---:|
| Solo | 19.37 | 220 | 0 | 0 | 16.5 s | 2 | 94.6% |
| Split | 19.36 | 221 | 0 | 0 | 16.3 s | 2 | 94.6% |
| Rush | 21.93 | 8 | 0 | 0 | 59.8 s | 3 | 94.7% |
| Explore | 24.70 | 275 | 5 | 5 | 21.8 s | 2 | 95.8% |
| Cautious | 20.08 | 225 | 1 | 1 | 16.5 s | 2 | 94.8% |

**Rush.** Passing the signals left this player up to 60 seconds between successes. The field check catches a missed skill before extraction. Answering Salvage plate in transit avoids a later detour.
**Explore.** Finding all five instances of Heat-sink vane and riding home added 5.3 minutes over the guided run. The mint home marker makes the destination legible before committing.
**Cautious.** The cautious player took the ward loop in the third sector, stunned nearby threats and finished in 20.08 minutes. Cold manifold gives the player a safe place to line up the final approach.
**Revision.** The old run let players pass long stretches with a single movement input. The side room added in this revision is Salt-road machine bay. Broken rows of cooling fins offer openings from different firing angles. This door now teaches Salvage plate, develops Heat shutter and asks for all three field responses before the final demonstration. The five new ward rooms connect from both sides. The quickest of these observed policies still had to finish the same three final encounters.

## 07 · The Waiting Room

Counter-Strike · classic mod / 1.0 era, 1999–2000

Read a station twice, once to find the porter and once to extract him. The quick approach is exposed on the way back. Aim for tension followed by relief when both reach the cordon.

Track a platform silhouette or take cover on the green line. Recover the left-luggage tag or stay close to the porter. Bring him through the return line or escort him through the concourse.

**Beats and graph**

Ticket kiosk (2) → Platform one (5) → Left luggage (7) → Signal crossing (4) → Porter's shelter (2) → Clock concourse (9) → A station timetable gives the first address of the machine that held the invitations (2).

Normal edges follow the spine, side passages and ward loop below. The first tool is the key to the chamber shutter. The mint return is a two-way shortcut earned in V. Sector exits are one-way after the required tools, transmission where present, and field practice. The final exit also requires three signature actions after shelter.

| Sector | Spine | Side passages | Ward loop | Route stations | Rooms / tiles | Optional rooms | Cycles | Dead ends and contents |
|---|---|---|---|---|---:|---:|---:|---|
| Ticket kiosk | `vqlkfbchmr` | `qrst lgm` | `cdh` | `qh` | 10 / 280 | 5/14 | 3 | t fuel signal, v arrival landmark |
| Platform one | `afkpuwrmhc` | `glq chmr fg` | `hic` | `kr` | 10 / 269 | 7/14 | 1 | a arrival landmark, q fuel signal |
| Left luggage | `edinsrqlgb` | `ihg lmn` | `lkg` | `il` | 11 / 262 | 3/13 | 3 | b extraction, e arrival landmark |
| Signal crossing | `pqrmhijoty` | `rsto ni` | `idj` | `ro` | 11 / 256 | 4/13 | 2 | n fuel signal, p arrival landmark, y extraction |
| Clock concourse | `wrqlgcdjon` | `rsmnh glf` | `qpl` | `rn` | 11 / 327 | 4/15 | 2 | f fuel signal, h tool / field reward, w arrival landmark |

- **Ticket kiosk** · Start v. Landmark courts qlfchr. Key Rescue kit at q → l opens the chamber shutter. Fight or aim rooms h Platform silhouettes. Puzzle or traversal rooms q Rescue cache. Hinted reward d. Shortcut d ↔ v, opened at d. Lock and exit r.
- **Platform one** · Start a. Landmark courts afkprm. Key Night porter at f → k opens the chamber shutter. Fight or aim rooms k Platform silhouettes. Puzzle or traversal rooms r Evacuation cordon. Hinted reward i. Shortcut i ↔ a, opened at i. Lock and exit c.
- **Left luggage** · Start e. Landmark courts dinqlg. Key Rescue kit at d → i opens the chamber shutter. Fight or aim rooms l Platform silhouettes. Puzzle or traversal rooms i Rescue cache, g Evacuation cordon. Hinted reward k. Shortcut k ↔ e, opened at k. Lock and exit b.
- **Signal crossing** · Start p. Landmark courts pqhmoy. Key Exit whistle at q → r opens the chamber shutter. Fight or aim rooms o Platform silhouettes. Puzzle or traversal rooms r Evacuation cordon. Hinted reward d. Shortcut d ↔ p, opened at d. Lock and exit y.
- **Clock concourse** · Start w. Landmark courts rqgcjn. Key Night porter at r → q opens the chamber shutter. Fight or aim rooms r Platform silhouettes. Puzzle or traversal rooms h Rescue cache, n Evacuation cordon. Hinted reward p. Shortcut p ↔ w, opened at p. Lock and exit n. Rest at Porter's shelter.

**Layout and critical walk**

```text
    Sector 1          Sector 2          Sector 3          Sector 4          Sector 5
   b* c* dV       aS    cE             bE    d* eS             dV             c* d*
f* g. h*          f* g. h. iV          g* h. i*             h* i* j*    f. g* h*    j*
k* l* m.          k* l. m.          kV l* m. n*             m* n. o*       l* m. nE o*
   q* rE s. t.    p* q. r*             q* r* s*       pS q* r* s. t.    pV q* r* s.
   vS             u*    w.                                        yE          wS
```

- **Ticket kiosk** · q Rescue kit → l Rescue kit finish → q Rescue cache → h Platform silhouettes → k Night porter → f Night porter finish → b Exit whistle → c Exit whistle finish → r Exit. A kiosk divides two approaches to the ticket hall. Locate the kit before calling the porter.
- **Platform one** · f Night porter → k Night porter finish → k Platform silhouettes → r Evacuation cordon → p Exit whistle → u Exit whistle finish → c Exit. Two parallel platforms meet at the footbridge. Escort the porter along the screened side.
- **Left luggage** · d Rescue kit → i Rescue kit finish → i Rescue cache → l Platform silhouettes → g Evacuation cordon → n Exit whistle → s Exit whistle finish → r Night porter → q Night porter finish → b Exit. A U-shaped luggage hall has a blind service aisle and a long public sightline.
- **Signal crossing** · q Exit whistle → r Exit whistle finish → r Evacuation cordon → o Platform silhouettes → m Night porter → h Night porter finish → i Rescue kit → j Rescue kit finish → y Exit. Across the tracks, two signal boxes overlook the crossing. Mark the exit before moving the group.
- **Clock concourse** · r Night porter → q Night porter finish → l Rescue kit → g Rescue kit finish → c Exit whistle → d Exit whistle finish → j Recover dispatch → o Answer dispatch → (42,62) Porter's shelter → r Platform silhouettes → h Rescue cache → n Evacuation cordon → n Exit. Extract through the clock concourse. The ring around the clock lets the party approach from either side.

**Content placement**

Left-luggage wicket · Two short banks of lockers separate the sheltered rescue aisle from the quick crossing.

```text
.....:.
.!XX.:.
.....:.
.:::::.
.:.....
.:.XX!.
.:.....
```

X is solid cover, : is the approach inlay, ! is an amber warning lamp, and the middle tile holds the optional prize.

- Platform silhouette appears on travel lines. Players aim and pulse for lift fuel and a brief burst of speed while travelling.
- Green cordon appears on travel lines. Players cross at ground height for lift fuel and a brief burst of speed while travelling.
- Rescue locker appears on travel lines. Players press Use for lift fuel and a brief burst of speed while travelling.
- Rescue cache is staged in a widened court. Use to open the team's rescue cache. The final sequence revisits this action after the shelter.
- Platform silhouettes is staged in a widened court. Identify and pulse the moving paper sentries. The final sequence revisits this action after the shelter.
- Evacuation cordon is staged in a widened court. Lead the sweep through the low green rescue cordon. The final sequence revisits this action after the shelter.
- Left-luggage tag sits between two entrances in the cover shown above. The orbiting amber lamp shows where contact will repel the player. Completing the indicated field skill awards a shared ward and opens the return line at the entrance.
- Porter's shelter sits three tiles from the first finale anchor. It refills both seats and gives the party control over when to begin.

**Verification and playtest**

Planned intensity **2 → 5 → 7 → 4 → 2 → 9 → 2**. Built mechanical intensity **5 → 5 → 7 → 5 → 2 → 8 → 2**.

The guided path crossed **53 rooms and 1,394 tiles**. Optional room content was **33%** (23 of 69). The five sectors contained **11 independent normal-corridor cycles**. Every authored room has a reachable field reward within **8 tiles** of its centre. A reward is available in every room. The longest guided stretch outside the sensing radius of a field signal or compulsory destination was **2 tiles**.

| Player | Minutes | Marks answered | Wards found | Return rides | Longest quiet stretch | Most room entries between rewards | Climax begins |
|---|---:|---:|---:|---:|---:|---:|---:|
| Solo | 18.83 | 193 | 0 | 0 | 17.2 s | 2 | 93.2% |
| Split | 18.83 | 193 | 0 | 0 | 17.2 s | 2 | 93.2% |
| Rush | 20.28 | 82 | 0 | 0 | 34.6 s | 3 | 93.3% |
| Explore | 25.57 | 244 | 5 | 5 | 17.2 s | 2 | 95.0% |
| Cautious | 20.22 | 204 | 1 | 1 | 17.2 s | 2 | 93.7% |

**Rush.** Passing the signals left this player up to 35 seconds between successes. The field check catches a missed skill before extraction. Answering Platform silhouette in transit avoids a later detour.
**Explore.** Finding all five instances of Left-luggage tag and riding home added 6.7 minutes over the guided run. The mint home marker makes the destination legible before committing.
**Cautious.** The cautious player took the ward loop in the third sector, stunned nearby threats and finished in 20.22 minutes. Porter's shelter gives the player a safe place to line up the final approach.
**Revision.** The old run let players pass long stretches with a single movement input. The side room added in this revision is Left-luggage wicket. Two short banks of lockers separate the sheltered rescue aisle from the quick crossing. This door now teaches Platform silhouette, develops Green cordon and asks for all three field responses before the final demonstration. The five new ward rooms connect from both sides. The quickest of these observed policies still had to finish the same three final encounters.

## 08 · Night Shift at Brineworks

Half-Life · 1998

Restore a chain of machinery that a technician can actually ride. Repairing the tram changes how you move the person who repairs it. Aim for the surprise of a machine becoming transportation.

Work the points or take the service walk. Search the switch room for a fuse or stick with the technician. Board the tram together or lead the technician past the pumps.

**Beats and graph**

Night tram (2) → Brine pumps (5) → Heat exchange (7) → Maintenance line (4) → Unpowered cab (2) → Last stop (9) → A tram transfer is timestamped one minute before every other delivery (2).

Normal edges follow the spine, side passages and ward loop below. The first tool is the key to the chamber shutter. The mint return is a two-way shortcut earned in V. Sector exits are one-way after the required tools, transmission where present, and field practice. The final exit also requires three signature actions after shelter.

| Sector | Spine | Side passages | Ward loop | Route stations | Rooms / tiles | Optional rooms | Cycles | Dead ends and contents |
|---|---|---|---|---|---:|---:|---:|---|
| Night tram | `uvwrmhcdij` | `mrst hgf` | `dei` | `rh` | 10 / 236 | 6/15 | 1 | f fuel signal, j extraction, t fuel signal, u arrival landmark |
| Brine pumps | `kfabchmnot` | `klm ejot` | `edj` | `km` | 11 / 280 | 4/14 | 2 | t extraction |
| Heat exchange | `ytsrqlghie` | `rmd glk` | `rwq` | `rh` | 11 / 283 | 3/14 | 1 | d fuel signal, e extraction, k fuel signal, y arrival landmark |
| Maintenance line | `fglqvwxsno` | `ghin rsm` | `hci` | `ln` | 12 / 308 | 5/15 | 2 | f arrival landmark, m fuel signal, o extraction, r fuel signal |
| Last stop | `bchmrwxytj` | `hmnol cg` | `nio` | `ht` | 12 / 332 | 5/15 | 1 | b arrival landmark, g fuel signal, j extraction, l fuel signal |

- **Night tram** · Start u. Landmark courts vrmhij. Key Pry bar at v → w opens the chamber shutter. Fight or aim rooms h Tram points. Puzzle or traversal rooms m Jammed panel. Hinted reward e. Shortcut e ↔ u, opened at e. Lock and exit j.
- **Brine pumps** · Start k. Landmark courts kfacho. Key Pump controls at f → a opens the chamber shutter. Fight or aim rooms k Pump bypass. Puzzle or traversal rooms m Jammed panel. Hinted reward d. Shortcut d ↔ k, opened at d. Lock and exit t.
- **Heat exchange** · Start y. Landmark courts tsqlhi. Key Technician at t → s opens the chamber shutter. Fight or aim rooms h Pump bypass, m Tram points. Puzzle or traversal rooms r Jammed panel. Hinted reward w. Shortcut w ↔ y, opened at w. Lock and exit e.
- **Maintenance line** · Start f. Landmark courts fglvsn. Key Pump controls at g → l opens the chamber shutter. Fight or aim rooms l Tram points, n Pump bypass. Puzzle or traversal rooms Pry bar. Hinted reward c. Shortcut c ↔ f, opened at c. Lock and exit o.
- **Last stop** · Start b. Landmark courts bchmwt. Key Technician at c → h opens the chamber shutter. Fight or aim rooms h Pump bypass, t Tram points. Puzzle or traversal rooms j Jammed panel. Hinted reward i. Shortcut i ↔ b, opened at i. Lock and exit j. Rest at Unpowered cab.

**Layout and critical walk**

```text
    Sector 1          Sector 2          Sector 3          Sector 4          Sector 5
      c. d* eV    a* b* c* dV e.             d. eE          cV             bS c*
f. g. h* i* jE    f*    h*    j.       g* h* i*       fS g* h. i.          g. h* iV jE
      m*          kS l. m* n* o*    k. l* m*             l* m. n* oE       l. m* n. o.
      r* s. t.                tE       q* r* s* t*       q* r. s*             r*    t*
uS v* w*                                  wV    yS       v* w* x*             w* x* y*
```

- **Night tram** · v Pry bar → w Pry bar finish → m Jammed panel → h Tram points → r Technician → m Technician finish → d Recover dispatch → i Answer dispatch → j Exit. The tram line runs into a switchback. The inspection walk leads behind the jammed points.
- **Brine pumps** · f Pump controls → a Pump controls finish → k Pump bypass → m Jammed panel → b Pry bar → c Pry bar finish → h Technician → m Technician finish → n Recover dispatch → o Answer dispatch → t Exit. Intake pipes lead up to a bank of pumps. The lock on the return manifold needs a technician.
- **Heat exchange** · t Technician → s Technician finish → r Jammed panel → h Pump bypass → m Tram points → r Pump controls → q Pump controls finish → l Pry bar → g Pry bar finish → h Recover dispatch → i Answer dispatch → e Exit. Two exchanger galleries fold around the same hot pipe. Reconnect the cold side from the far valve.
- **Maintenance line** · g Pump controls → l Pump controls finish → l Tram points → n Pump bypass → q Technician → v Technician finish → w Pry bar → x Pry bar finish → s Recover dispatch → n Answer dispatch → o Exit. The maintenance line dives under the tram. Follow its elbows to the buried points cabinet.
- **Last stop** · c Technician → h Technician finish → m Pry bar → r Pry bar finish → w Pump controls → x Pump controls finish → y Recover dispatch → t Answer dispatch → (44,29) Unpowered cab → h Pump bypass → t Tram points → j Jammed panel → j Exit. The last stop lies beyond the track fan. Route the restored tram from the platform cabin.

**Content placement**

Tram switch room · The service corridor winds past two pump casings to the points cabinet.

```text
..:....
.!:..X.
..:.XX.
..:::..
.XX.:..
.X..:!.
....:..
```

X is solid cover, : is the approach inlay, ! is an amber warning lamp, and the middle tile holds the optional prize.

- Service panel appears on travel lines. Players press Use for lift fuel and a brief burst of speed while travelling.
- Live points appears on travel lines. Players pulse on the bright beat for lift fuel and a brief burst of speed while travelling.
- Breaker contact appears on travel lines. Players aim and pulse for lift fuel and a brief burst of speed while travelling.
- Jammed panel is staged in a widened court. Use the pry bar on the service hatch. The final sequence revisits this action after the shelter.
- Pump bypass is staged in a widened court. Set the bypass dial and pulse the circuit. The final sequence revisits this action after the shelter.
- Tram points is staged in a widened court. Pulse while the live contact lines up with the points. The final sequence revisits this action after the shelter.
- Switch-room fuse sits between two entrances in the cover shown above. The orbiting amber lamp shows where contact will repel the player. Completing the indicated field skill awards a shared ward and opens the return line at the entrance.
- Unpowered cab sits three tiles from the first finale anchor. It refills both seats and gives the party control over when to begin.

**Verification and playtest**

Planned intensity **2 → 5 → 7 → 4 → 2 → 9 → 2**. Built mechanical intensity **6 → 5 → 8 → 6 → 2 → 9 → 2**.

The guided path crossed **56 rooms and 1,439 tiles**. Optional room content was **32%** (23 of 73). The five sectors contained **7 independent normal-corridor cycles**. Every authored room has a reachable field reward within **8 tiles** of its centre. A reward is available in every room. The longest guided stretch outside the sensing radius of a field signal or compulsory destination was **1 tiles**.

| Player | Minutes | Marks answered | Wards found | Return rides | Longest quiet stretch | Most room entries between rewards | Climax begins |
|---|---:|---:|---:|---:|---:|---:|---:|
| Solo | 19.38 | 198 | 0 | 0 | 14.8 s | 2 | 92.0% |
| Split | 19.36 | 198 | 0 | 0 | 14.8 s | 2 | 92.0% |
| Rush | 21.92 | 8 | 0 | 0 | 78.2 s | 5 | 92.5% |
| Explore | 25.86 | 259 | 5 | 5 | 14.8 s | 2 | 94.1% |
| Cautious | 20.33 | 206 | 1 | 1 | 14.8 s | 2 | 92.5% |

**Rush.** Passing the signals left this player up to 78 seconds between successes. The field check catches a missed skill before extraction. Answering Service panel in transit avoids a later detour.
**Explore.** Finding all five instances of Switch-room fuse and riding home added 6.5 minutes over the guided run. The mint home marker makes the destination legible before committing.
**Cautious.** The cautious player took the ward loop in the third sector, stunned nearby threats and finished in 20.33 minutes. Unpowered cab gives the player a safe place to line up the final approach.
**Revision.** The old run let players pass long stretches with a single movement input. The side room added in this revision is Tram switch room. The service corridor winds past two pump casings to the points cabinet. This door now teaches Service panel, develops Live points and asks for all three field responses before the final demonstration. The five new ward rooms connect from both sides. The quickest of these observed policies still had to finish the same three final encounters.

## 09 · Flag in the Whiteout

Starsiege: Tribes · 1998

Turn descent speed and a short jet climb into a flag run. The pennant has to come back through terrain first learned empty-handed. Aim for a clean linked descent and crest.

Skim the trough or climb the crest. Peel off for the race pennant or keep the line. Use the unlocked return or ride the ridge home.

**Beats and graph**

Home bowl (2) → Windward ridge (5) → Kite battery (7) → Pennant tower (4) → Lee of the mast (2) → Return crest (9) → The windsock’s cloth is cut from one of the old hall curtains (2).

Normal edges follow the spine, side passages and ward loop below. The first tool is the key to the chamber shutter. The mint return is a two-way shortcut earned in V. Sector exits are one-way after the required tools, transmission where present, and field practice. The final exit also requires three signature actions after shelter.

| Sector | Spine | Side passages | Ward loop | Route stations | Rooms / tiles | Optional rooms | Cycles | Dead ends and contents |
|---|---|---|---|---|---:|---:|---:|---|
| Home bowl | `wqkfbhnsym` | `wsm mq` | `klf` | `qm` | 10 / 296 | 2/11 | 4 | None |
| Windward ridge | `uqmiejoswr` | `mgs qrs` | `qvr` | `qs` | 8 / 217 | 5/12 | 4 | u arrival landmark |
| Kite battery | `aghiotxrlf` | `gmnr hdc` | `rql` | `gr` | 10 / 309 | 6/15 | 2 | a arrival landmark, c fuel signal, f extraction |
| Pennant tower | `vqlgcinsxw` | `lmn qrs` | `ijn` | `qx` | 11 / 214 | 3/13 | 3 | v arrival landmark, w extraction |
| Return crest | `eimqupkgcw` | `inr lmh` | `kfg` | `iw` | 11 / 263 | 6/15 | 1 | e arrival landmark, h fuel signal, l fuel signal, r fuel signal, w extraction |

- **Home bowl** · Start w. Landmark courts wqbhsy. Key Jet crest at q opens the chamber shutter. Fight or aim rooms use the active tool sentry. Puzzle or traversal rooms q Ski gate, m Jet crest. Hinted reward l. Shortcut l ↔ w, opened at l. Lock and exit m.
- **Windward ridge** · Start u. Landmark courts uqieow. Key Disc target at q → m opens the chamber shutter. Fight or aim rooms use the active tool sentry. Puzzle or traversal rooms m Jet crest, s Ski gate. Hinted reward v. Shortcut v ↔ u, opened at v. Lock and exit r.
- **Kite battery** · Start a. Landmark courts agioxl. Key Disc target at g → h opens the chamber shutter. Fight or aim rooms use the active tool sentry. Puzzle or traversal rooms g Disc range, r Ski gate. Hinted reward q. Shortcut q ↔ a, opened at q. Lock and exit f.
- **Pennant tower** · Start v. Landmark courts qgcnsw. Key Disc target at q → l opens the chamber shutter. Fight or aim rooms use the active tool sentry. Puzzle or traversal rooms l Ski gate, n Jet crest. Hinted reward j. Shortcut j ↔ v, opened at j. Lock and exit w.
- **Return crest** · Start e. Landmark courts eimukw. Key Tower pennant at i → m opens the chamber shutter. Fight or aim rooms use the active tool sentry. Puzzle or traversal rooms i Disc range, m Jet crest, w Ski gate. Hinted reward f. Shortcut f ↔ e, opened at f. Lock and exit w. Rest at Lee of the mast.

**Layout and critical walk**

```text
    Sector 1          Sector 2          Sector 3          Sector 4          Sector 5
   b*                         e.    aS    c. d.             c*                c*    eS
f*    h*             g.    i* j.    fE g* h* i*          g*    i* jV    fV g* h. i*
k. lV mE n*             m*    o.       l* m. n. o.       l* m. n*       k* l. m* n.
   q*    s*          q* rE s*          qV r*    t*       q* r. s*       p* q* r.
      wS    y*    uS vV w*                   x*          vS wE x*       u.    wE
```

- **Home bowl** · q Jet crest → q Ski gate → m Jet crest → f Tower pennant → b Tower pennant finish → h Disc target → n Disc target finish → s Recover dispatch → y Answer dispatch → m Exit. Two lips enclose the home bowl. Take the high crest on the approach and ski the low line back.
- **Windward ridge** · q Disc target → m Disc target finish → m Jet crest → s Ski gate → i Jet crest → s Recover dispatch → w Answer dispatch → r Exit. A diagonal ridge splits the windward slope. The lee approach trades speed for shelter.
- **Kite battery** · g Disc target → h Disc target finish → g Disc range → r Ski gate → i Jet crest → t Tower pennant → x Tower pennant finish → r Recover dispatch → l Answer dispatch → f Exit. The kite battery stands at the bend of the snow saddle, visible from both flanks.
- **Pennant tower** · q Disc target → l Disc target finish → l Ski gate → n Jet crest → g Tower pennant → c Tower pennant finish → i Jet crest → s Recover dispatch → x Answer dispatch → w Exit. Twin ridges converge on the flag tower. The pennant must come down the other side.
- **Return crest** · i Tower pennant → m Tower pennant finish → q Jet crest → p Disc target → k Disc target finish → g Recover dispatch → c Answer dispatch → (50,25) Lee of the mast → i Disc range → m Jet crest → w Ski gate → w Exit. Break from the exposed summit and carry the flag through the long home bowl.

**Content placement**

Pennant bowl · Two pylons occupy the shoulders of a broad landing strip and leave room for a low pass.

```text
...:...
.!X:.!.
..:::..
:::::::
..:::..
.!.:X!.
...:...
```

X is solid cover, : is the approach inlay, ! is an amber warning lamp, and the middle tile holds the optional prize.

- Ski trough appears on travel lines. Players cross at ground height for lift fuel and a brief burst of speed while travelling.
- Jet crest appears on travel lines. Players lift through the crown for lift fuel and a brief burst of speed while travelling.
- Disc gate appears on travel lines. Players aim and pulse for lift fuel and a brief burst of speed while travelling.
- Ski gate is staged in a widened court. Cross the low ridge gate at ground level. The final sequence revisits this action after the shelter.
- Jet crest is staged in a widened court. Lift through the summit ring. The final sequence revisits this action after the shelter.
- Disc range is staged in a widened court. Lead the three moving disc targets. The final sequence revisits this action after the shelter.
- Race pennant sits between two entrances in the cover shown above. The orbiting amber lamp shows where contact will repel the player. Completing the indicated field skill awards a shared ward and opens the return line at the entrance.
- Lee of the mast sits three tiles from the first finale anchor. It refills both seats and gives the party control over when to begin.

**Verification and playtest**

Planned intensity **2 → 5 → 7 → 4 → 2 → 9 → 2**. Built mechanical intensity **5 → 5 → 6 → 5 → 2 → 8 → 2**.

The guided path crossed **50 rooms and 1,299 tiles**. Optional room content was **33%** (22 of 66). The five sectors contained **14 independent normal-corridor cycles**. Every authored room has a reachable field reward within **9 tiles** of its centre. A reward is available in every room. The longest guided stretch outside the sensing radius of a field signal or compulsory destination was **20 tiles**.

| Player | Minutes | Marks answered | Wards found | Return rides | Longest quiet stretch | Most room entries between rewards | Climax begins |
|---|---:|---:|---:|---:|---:|---:|---:|
| Solo | 18.16 | 124 | 0 | 0 | 25.3 s | 2 | 95.7% |
| Split | 18.16 | 124 | 0 | 0 | 25.3 s | 2 | 95.7% |
| Rush | 19.99 | 54 | 0 | 0 | 47.7 s | 3 | 95.7% |
| Explore | 21.97 | 145 | 5 | 5 | 46.3 s | 2 | 96.4% |
| Cautious | 18.83 | 127 | 1 | 1 | 35.3 s | 2 | 95.8% |

**Rush.** Passing the signals left this player up to 48 seconds between successes. The field check catches a missed skill before extraction. Answering Ski trough in transit avoids a later detour.
**Explore.** Finding all five instances of Race pennant and riding home added 3.8 minutes over the guided run. The mint home marker makes the destination legible before committing.
**Cautious.** The cautious player took the ward loop in the third sector, stunned nearby threats and finished in 18.83 minutes. Lee of the mast gives the player a safe place to line up the final approach.
**Revision.** The old run let players pass long stretches with a single movement input. The side room added in this revision is Pennant bowl. Two pylons occupy the shoulders of a broad landing strip and leave room for a low pass. This door now teaches Ski trough, develops Jet crest and asks for all three field responses before the final demonstration. The five new ward rooms connect from both sides. The quickest of these observed policies still had to finish the same three final encounters.

## 10 · People Under Neon

Syndicate Wars · 1996

Read the city as a network of couriers and operable facades. A frontage that hid the route folds away once the stamp is charged. Aim for the pleasure of seeing through the city plan.

Follow the messenger light or take the street. Go behind the hoardings for a dispatch seal or stay with the courier. Use the back line together or circle the visible block.

**Beats and graph**

Sign exchange (2) → Courier lane (5) → Chrome arcade (7) → Tram depot (4) → Unmarked alley (2) → False boulevard (9) → A courier gives you a stamped envelope bearing the order to hold the mail (2).

Normal edges follow the spine, side passages and ward loop below. The first tool is the key to the chamber shutter. The mint return is a two-way shortcut earned in V. Sector exits are one-way after the required tools, transmission where present, and field practice. The final exit also requires three signature actions after shelter.

| Sector | Spine | Side passages | Ward loop | Route stations | Rooms / tiles | Optional rooms | Cycles | Dead ends and contents |
|---|---|---|---|---|---:|---:|---:|---|
| Sign exchange | `acejonmlkf` | `chm fghij` | `cbh` | `cl` | 12 / 310 | 4/14 | 4 | a arrival landmark |
| Courier lane | `uvqrmhidcb` | `rsto hgf` | `mlh` | `qi` | 10 / 248 | 6/16 | 1 | b extraction, f fuel signal, o fuel signal, u arrival landmark |
| Chrome arcade | `ojedchmrqp` | `jihin rqv` | `rwq` | `jm` | 11 / 249 | 4/14 | 2 | n fuel signal, o arrival landmark, p extraction, v fuel signal |
| Tram depot | `fkpuwrsnie` | `klmno hms` | `lgm` | `ks` | 12 / 306 | 6/15 | 3 | e extraction, f arrival landmark, h fuel signal, o fuel signal |
| False boulevard | `bglqvxyojd` | `lmn jihg` | `lkq` | `lo` | 14 / 358 | 4/15 | 2 | b arrival landmark, d extraction, n tool / field reward |

- **Sign exchange** · Start a. Landmark courts aceomk. Key Rally marker at c → e opens the chamber shutter. Fight or aim rooms c Demolition stamp. Puzzle or traversal rooms l Messenger rally. Hinted reward b. Shortcut b ↔ a, opened at b. Lock and exit f.
- **Courier lane** · Start u. Landmark courts vqrhdb. Key Courier beacon at v → q opens the chamber shutter. Fight or aim rooms i Street intercept. Puzzle or traversal rooms q Messenger rally. Hinted reward l. Shortcut l ↔ u, opened at l. Lock and exit b.
- **Chrome arcade** · Start o. Landmark courts ojchqp. Key Facade stamp at j → e opens the chamber shutter. Fight or aim rooms j Street intercept, m Demolition stamp. Puzzle or traversal rooms h Messenger rally. Hinted reward w. Shortcut w ↔ o, opened at w. Lock and exit p.
- **Tram depot** · Start f. Landmark courts kpwrsn. Key Courier beacon at k → p opens the chamber shutter. Fight or aim rooms s Demolition stamp. Puzzle or traversal rooms k Messenger rally. Hinted reward g. Shortcut g ↔ f, opened at g. Lock and exit e.
- **False boulevard** · Start b. Landmark courts bgqvyo. Key Facade stamp at g → l opens the chamber shutter. Fight or aim rooms l Demolition stamp, o Street intercept. Puzzle or traversal rooms n Messenger rally. Hinted reward k. Shortcut k ↔ b, opened at k. Lock and exit d. Rest at Unmarked alley.

**Layout and critical walk**

```text
    Sector 1          Sector 2          Sector 3          Sector 4          Sector 5
aS bV c*    e*       bE c* d*             c* d* e*                eE       bS    dE
fE g. h. i. j*    f. g. h* i*             h* i. j*    fS gV h. i*          g* h. i. j*
k* l* m* n* o*       lV m*    o.          m* n. oS    k* l. m. n* o.    kV l* m. n* o*
                     q* r* s. t.    pE q* r*          p*    r. s*          q*
                  uS v*                v. wV          u*    w*             v*    x* y*
```

- **Sign exchange** · c Rally marker → e Rally marker finish → c Demolition stamp → l Messenger rally → j Courier beacon → o Courier beacon finish → n Facade stamp → m Facade stamp finish → l Recover dispatch → k Answer dispatch → f Exit. Billboards conceal the short pedestrian alley. Use the broad boulevard to gather the couriers.
- **Courier lane** · v Courier beacon → q Courier beacon finish → q Messenger rally → i Street intercept → r Rally marker → m Rally marker finish → h Facade stamp → i Facade stamp finish → d Recover dispatch → c Answer dispatch → b Exit. A courier lane doglegs behind a row of facades. The rendezvous sits above its blind corner.
- **Chrome arcade** · j Facade stamp → e Facade stamp finish → j Street intercept → m Demolition stamp → h Messenger rally → d Courier beacon → c Courier beacon finish → h Rally marker → m Rally marker finish → r Recover dispatch → q Answer dispatch → p Exit. Cut through the arcade sign shops to the rally square. A folded frontage opens the sightline.
- **Tram depot** · k Courier beacon → p Courier beacon finish → k Messenger rally → s Demolition stamp → u Facade stamp → w Facade stamp finish → n Recover dispatch → i Answer dispatch → e Exit. The tram depot funnels into separate loading and passenger streets. Rally at the shared stop.
- **False boulevard** · g Facade stamp → l Facade stamp finish → q Rally marker → v Rally marker finish → x Courier beacon → y Courier beacon finish → o Recover dispatch → j Answer dispatch → (24,43) Unmarked alley → l Demolition stamp → o Street intercept → n Messenger rally → d Exit. Walk the fake boulevard to its end. Stamp the frontage and bring the messengers through the block.

**Content placement**

Courier print shop · Shelves form a false frontage. A break in the shelving gives a second view of the seal.

```text
....:..
.!..:X.
.XX.:..
..::::.
..:.XX.
.X:..!.
..:....
```

X is solid cover, : is the approach inlay, ! is an amber warning lamp, and the middle tile holds the optional prize.

- Courier drop appears on travel lines. Players press Use for lift fuel and a brief burst of speed while travelling.
- Facade seal appears on travel lines. Players charge and release on the plate for lift fuel and a brief burst of speed while travelling.
- Rally light appears on travel lines. Players pulse on the bright beat for lift fuel and a brief burst of speed while travelling.
- Messenger rally is staged in a widened court. Use the consent beacon to bring the couriers together. The final sequence revisits this action after the shelter.
- Demolition stamp is staged in a widened court. Hold the stamp on the condemned sign until the facade folds. The final sequence revisits this action after the shelter.
- Street intercept is staged in a widened court. Pulse when the messenger signal flashes green. The final sequence revisits this action after the shelter.
- Dispatch seal sits between two entrances in the cover shown above. The orbiting amber lamp shows where contact will repel the player. Completing the indicated field skill awards a shared ward and opens the return line at the entrance.
- Unmarked alley sits three tiles from the first finale anchor. It refills both seats and gives the party control over when to begin.

**Verification and playtest**

Planned intensity **2 → 5 → 7 → 4 → 2 → 9 → 2**. Built mechanical intensity **5 → 6 → 8 → 5 → 2 → 9 → 2**.

The guided path crossed **59 rooms and 1,471 tiles**. Optional room content was **32%** (24 of 74). The five sectors contained **12 independent normal-corridor cycles**. Every authored room has a reachable field reward within **7 tiles** of its centre. A reward is available in every room. The longest guided stretch outside the sensing radius of a field signal or compulsory destination was **2 tiles**.

| Player | Minutes | Marks answered | Wards found | Return rides | Longest quiet stretch | Most room entries between rewards | Climax begins |
|---|---:|---:|---:|---:|---:|---:|---:|
| Solo | 19.87 | 211 | 1 | 0 | 13.1 s | 2 | 91.2% |
| Split | 19.87 | 211 | 1 | 0 | 13.1 s | 2 | 91.2% |
| Rush | 23.08 | 8 | 0 | 0 | 68.9 s | 4 | 91.7% |
| Explore | 23.64 | 246 | 5 | 5 | 13.6 s | 2 | 92.6% |
| Cautious | 21.13 | 225 | 2 | 1 | 13.1 s | 2 | 91.7% |

**Rush.** Passing the signals left this player up to 69 seconds between successes. The field check catches a missed skill before extraction. Answering Courier drop in transit avoids a later detour.
**Explore.** Finding all five instances of Dispatch seal and riding home added 3.8 minutes over the guided run. The mint home marker makes the destination legible before committing.
**Cautious.** The cautious player took the ward loop in the third sector, stunned nearby threats and finished in 21.13 minutes. Unmarked alley gives the player a safe place to line up the final approach.
**Revision.** The old run let players pass long stretches with a single movement input. The side room added in this revision is Courier print shop. Shelves form a false frontage. A break in the shelving gives a second view of the seal. This door now teaches Courier drop, develops Facade seal and asks for all three field responses before the final demonstration. The five new ward rooms connect from both sides. The quickest of these observed policies still had to finish the same three final encounters.

## 11 · The Mine That Lost Down

Descent · 1995 · original full release

Find a route through a mine by altitude and by leaving bearings. The rescue route reverses the high and low approach. Aim for disorientation that resolves into mastery.

Fly high to the hatch or follow low bearings. Duck into the survey pocket for a gyroscope or keep the core in view. Lift into the airlock or retrace the mine.

**Beats and graph**

Spherical dock (2) → Inverted shaft (5) → Rescue drift (7) → Core junction (4) → Pressure refuge (2) → Escape channel (9) → A miner returns an ornament that always points toward the hall, even when upside down (2).

Normal edges follow the spine, side passages and ward loop below. The first tool is the key to the chamber shutter. The mint return is a two-way shortcut earned in V. Sector exits are one-way after the required tools, transmission where present, and field practice. The final exit also requires three signature actions after shelter.

| Sector | Spine | Side passages | Ward loop | Route stations | Rooms / tiles | Optional rooms | Cycles | Dead ends and contents |
|---|---|---|---|---|---:|---:|---:|---|
| Spherical dock | `mhcbfkpvwr` | `mnr qlh` | `fgk` | `mr` | 10 / 335 | 4/14 | 2 | q fuel signal |
| Inverted shaft | `eihmrsxwvq` | `hgl nmr` | `idh` | `hr` | 10 / 196 | 4/14 | 1 | e arrival landmark, l fuel signal, n fuel signal, q extraction |
| Rescue drift | `upkghinsty` | `klmnr ijo` | `lqm` | `hn` | 10 / 264 | 6/16 | 2 | o fuel signal, r fuel signal, u arrival landmark, y extraction |
| Core junction | `aflqrmnjed` | `lmh qvw` | `lgm` | `lm` | 10 / 218 | 4/14 | 2 | a arrival landmark, d extraction, h fuel signal, w fuel signal |
| Escape channel | `ysmgabciow` | `mnr hgl` | `gfa` | `sm` | 10 / 420 | 5/15 | 1 | h fuel signal, l fuel signal, r fuel signal, w extraction, y arrival landmark |

- **Spherical dock** · Start m. Landmark courts mhckvr. Key Map breadcrumb at h → c opens the chamber shutter. Fight or aim rooms use the active tool sentry. Puzzle or traversal rooms m Upper pod hatch, r Inverted airway. Hinted reward g. Shortcut g ↔ m, opened at g. Lock and exit r.
- **Inverted shaft** · Start e. Landmark courts eimrxv. Key Rescue pod at i → h opens the chamber shutter. Fight or aim rooms use the active tool sentry. Puzzle or traversal rooms h Inverted airway, r Upper pod hatch. Hinted reward d. Shortcut d ↔ e, opened at d. Lock and exit q.
- **Rescue drift** · Start u. Landmark courts ukghsy. Key Rescue pod at p → k opens the chamber shutter. Fight or aim rooms use the active tool sentry. Puzzle or traversal rooms h Upper pod hatch, n Inverted airway. Hinted reward q. Shortcut q ↔ u, opened at q. Lock and exit y.
- **Core junction** · Start a. Landmark courts flrmje. Key Core switch at f → l opens the chamber shutter. Fight or aim rooms l Core breaker. Puzzle or traversal rooms m Upper pod hatch. Hinted reward g. Shortcut g ↔ a, opened at g. Lock and exit d.
- **Escape channel** · Start y. Landmark courts ysmciw. Key Core switch at s → m opens the chamber shutter. Fight or aim rooms s Core breaker. Puzzle or traversal rooms m Upper pod hatch, g Inverted airway. Hinted reward f. Shortcut f ↔ y, opened at f. Lock and exit w. Rest at Pressure refuge.

**Layout and critical walk**

```text
    Sector 1          Sector 2          Sector 3          Sector 4          Sector 5
   b* c*                   dV eS                      aS       dE e*    a* b* c*
f* gV h*             g. h* i*          g* h* i* j.    f* gV h.    j*    fV g* h. i*
k* l. mS n.          l. m* n.       k* l. m. n* o.       l* m* n*          l. m* n. o*
p* q. rE             qE r* s*       p* qV r. s* t*       q* r*                r. s*
   v* w*             v* w* x*       uS          yE       v. w.                wE    yS
```

- **Spherical dock** · h Map breadcrumb → c Map breadcrumb finish → m Upper pod hatch → r Inverted airway → b Rescue pod → f Rescue pod finish → k Core switch → p Core switch finish → v Recover dispatch → w Answer dispatch → r Exit. Radial bores meet at the spherical dock. Drop a breadcrumb before taking the rescue spoke.
- **Inverted shaft** · i Rescue pod → h Rescue pod finish → h Inverted airway → r Upper pod hatch → m Core switch → r Core switch finish → s Map breadcrumb → x Map breadcrumb finish → w Recover dispatch → v Answer dispatch → q Exit. Follow the folded shaft down past its returning bore. Look up to find the pod latch.
- **Rescue drift** · p Rescue pod → k Rescue pod finish → h Upper pod hatch → n Inverted airway → g Map breadcrumb → h Map breadcrumb finish → i Core switch → n Core switch finish → s Recover dispatch → t Answer dispatch → y Exit. Three rescue drifts branch around a rock plug. The far airlock sits behind the plug.
- **Core junction** · f Core switch → l Core switch finish → l Core breaker → m Upper pod hatch → q Map breadcrumb → r Map breadcrumb finish → m Rescue pod → n Rescue pod finish → j Recover dispatch → e Answer dispatch → d Exit. Trace the power bore to its core junction. The return bore has a separate opening.
- **Escape channel** · s Core switch → m Core switch finish → g Rescue pod → a Rescue pod finish → b Map breadcrumb → c Map breadcrumb finish → i Recover dispatch → o Answer dispatch → (56,59) Pressure refuge → s Core breaker → m Upper pod hatch → g Inverted airway → w Exit. Cut across the core and escape along the outer exhaust. The lit navigation anchors lead home.

**Content placement**

Gyroscope bore · Alternating ribs mark the two bore mouths, making the return bearing legible from the centre.

```text
..:::..
.X..:!.
.X..:..
..:::..
..:..X.
.!:..X.
..:::..
```

X is solid cover, : is the approach inlay, ! is an amber warning lamp, and the middle tile holds the optional prize.

- Upper airway appears on travel lines. Players lift through the crown for lift fuel and a brief burst of speed while travelling.
- Inverted bore appears on travel lines. Players cross at ground height for lift fuel and a brief burst of speed while travelling.
- Bearing lamp appears on travel lines. Players aim and pulse for lift fuel and a brief burst of speed while travelling.
- Upper pod hatch is staged in a widened court. Pitch up or lift through the high pod hatch. The final sequence revisits this action after the shelter.
- Inverted airway is staged in a widened court. Descend and fly through the low exhaust ring. The final sequence revisits this action after the shelter.
- Core breaker is staged in a widened court. Tune the breaker and pulse the marked core feed. The final sequence revisits this action after the shelter.
- Survey gyroscope sits between two entrances in the cover shown above. The orbiting amber lamp shows where contact will repel the player. Completing the indicated field skill awards a shared ward and opens the return line at the entrance.
- Pressure refuge sits three tiles from the first finale anchor. It refills both seats and gives the party control over when to begin.

**Verification and playtest**

Planned intensity **2 → 5 → 7 → 4 → 2 → 9 → 2**. Built mechanical intensity **5 → 5 → 6 → 5 → 2 → 8 → 2**.

The guided path crossed **50 rooms and 1,433 tiles**. Optional room content was **32%** (23 of 73). The five sectors contained **8 independent normal-corridor cycles**. Every authored room has a reachable field reward within **8 tiles** of its centre. A reward is available in every room. The longest guided stretch outside the sensing radius of a field signal or compulsory destination was **2 tiles**.

| Player | Minutes | Marks answered | Wards found | Return rides | Longest quiet stretch | Most room entries between rewards | Climax begins |
|---|---:|---:|---:|---:|---:|---:|---:|
| Solo | 18.76 | 239 | 0 | 0 | 10.0 s | 2 | 89.3% |
| Split | 18.76 | 239 | 0 | 0 | 10.0 s | 2 | 89.3% |
| Rush | 20.78 | 80 | 0 | 0 | 31.8 s | 2 | 89.1% |
| Explore | 23.53 | 305 | 5 | 5 | 10.0 s | 2 | 91.5% |
| Cautious | 19.65 | 249 | 1 | 1 | 10.0 s | 2 | 89.8% |

**Rush.** Passing the signals left this player up to 32 seconds between successes. The field check catches a missed skill before extraction. Answering Upper airway in transit avoids a later detour.
**Explore.** Finding all five instances of Survey gyroscope and riding home added 4.8 minutes over the guided run. The mint home marker makes the destination legible before committing.
**Cautious.** The cautious player took the ward loop in the third sector, stunned nearby threats and finished in 19.65 minutes. Pressure refuge gives the player a safe place to line up the final approach.
**Revision.** The old run let players pass long stretches with a single movement input. The side room added in this revision is Gyroscope bore. Alternating ribs mark the two bore mouths, making the return bearing legible from the centre. This door now teaches Upper airway, develops Inverted bore and asks for all three field responses before the final demonstration. The five new ward rooms connect from both sides. The quickest of these observed policies still had to finish the same three final encounters.

## 12 · The Parish Below

Diablo · 1996/97 · original PC release

Descend for an unnamed relic, identify it and bind a way home. The room that looks like loot is also the safest route back up. Aim for curiosity competing with the urge to leave.

Test the ward pulse or follow the bare stair. Identify the votive in the side chapel or protect the main relic. Use the return circle or keep exploring the crypt.

**Beats and graph**

Sexton yard (2) → Wax crypt (5) → Undercroft (7) → Sealed nave (4) → Warm vestry (2) → Parish stair (9) → The guest register contains everyone’s name (2).

Normal edges follow the spine, side passages and ward loop below. The first tool is the key to the chamber shutter. The mint return is a two-way shortcut earned in V. Sector exits are one-way after the required tools, transmission where present, and field practice. The final exit also requires three signature actions after shelter.

| Sector | Spine | Side passages | Ward loop | Route stations | Rooms / tiles | Optional rooms | Cycles | Dead ends and contents |
|---|---|---|---|---|---:|---:|---:|---|
| Sexton yard | `chmlkpuvwr` | `mno hg` | `him` | `hw` | 10 / 248 | 5/14 | 1 | c arrival landmark, g fuel signal, o fuel signal, r extraction |
| Wax crypt | `upkfbchmno` | `klm chd` | `him` | `km` | 11 / 174 | 4/13 | 2 | d fuel signal, o extraction, u arrival landmark |
| Undercroft | `edinmlqvwx` | `ihg msr` | `qpv` | `iv` | 11 / 253 | 4/15 | 1 | e arrival landmark, g fuel signal, r fuel signal, x extraction |
| Sealed nave | `kfacejonsr` | `chg lmn` | `cdh` | `cr` | 11 / 300 | 7/15 | 1 | g fuel signal, k arrival landmark, l fuel signal, r extraction |
| Parish stair | `ytoicbglqw` | `onm qrs` | `ghl` | `iw` | 11 / 534 | 4/15 | 1 | m fuel signal, s fuel signal, w extraction, y arrival landmark |

- **Sexton yard** · Start c. Landmark courts chlkvr. Key Return scroll at h → m opens the chamber shutter. Fight or aim rooms w Identification shrine. Puzzle or traversal rooms h Relic drop. Hinted reward i. Shortcut i ↔ c, opened at i. Lock and exit r.
- **Wax crypt** · Start u. Landmark courts ukfbho. Key Unnamed relic at p → k opens the chamber shutter. Fight or aim rooms k Ward seal. Puzzle or traversal rooms m Relic drop. Hinted reward i. Shortcut i ↔ u, opened at i. Lock and exit o.
- **Undercroft** · Start e. Landmark courts ednlvx. Key Identification shrine at d → i opens the chamber shutter. Fight or aim rooms v Identification shrine, s Ward seal. Puzzle or traversal rooms i Relic drop. Hinted reward p. Shortcut p ↔ e, opened at p. Lock and exit x.
- **Sealed nave** · Start k. Landmark courts kaceor. Key Identification shrine at f → a opens the chamber shutter. Fight or aim rooms c Identification shrine, r Ward seal. Puzzle or traversal rooms Unnamed relic. Hinted reward d. Shortcut d ↔ k, opened at d. Lock and exit r.
- **Parish stair** · Start y. Landmark courts yoicqw. Key Unnamed relic at t → o opens the chamber shutter. Fight or aim rooms i Ward seal, n Identification shrine. Puzzle or traversal rooms w Relic drop. Hinted reward h. Shortcut h ↔ y, opened at h. Lock and exit w. Rest at Warm vestry.

**Layout and critical walk**

```text
    Sector 1          Sector 2          Sector 3          Sector 4          Sector 5
      cS             b* c* d.                d* eS    a*    c* dV e*       b* c*
   g. h* iV       f*    h* iV          g. h. i*       f* g. h.    j*       g* hV i*
k* l* m* n. o.    k* l. m* n. oE       l* m* n*       kS l. m. n. o*       l* m. n* o*
p*    rE          p*                pV q* r. s*             rE s.          q* r. s. t*
u* v. w*          uS                   v* w* xE                               wE    yS
```

- **Sexton yard** · h Return scroll → m Return scroll finish → h Relic drop → w Identification shrine → l Identification shrine → k Identification shrine finish → p Unnamed relic → u Unnamed relic finish → r Exit. Leave the sexton's yard through the cloister and bring the first find to the shrine.
- **Wax crypt** · p Unnamed relic → k Unnamed relic finish → k Ward seal → m Relic drop → f Identification shrine → b Identification shrine finish → c Return scroll → h Return scroll finish → o Exit. Side burial cells branch from a bent crypt spine. Identify what the dead left at its far end.
- **Undercroft** · d Identification shrine → i Identification shrine finish → i Relic drop → v Identification shrine → s Ward seal → n Unnamed relic → m Unnamed relic finish → l Return scroll → q Return scroll finish → v Recover dispatch → w Answer dispatch → x Exit. The undercroft doubles back under the chancel. A return scroll links the two walks.
- **Sealed nave** · f Identification shrine → a Identification shrine finish → c Identification shrine → r Ward seal → c Return scroll → e Return scroll finish → j Unnamed relic → o Unnamed relic finish → r Exit. The nave wraps around sealed side chapels. Carry the identified relic along the ambulatory.
- **Parish stair** · t Unnamed relic → o Unnamed relic finish → i Return scroll → c Return scroll finish → b Identification shrine → g Identification shrine finish → l Recover dispatch → q Answer dispatch → (44,23) Warm vestry → i Ward seal → w Relic drop → n Identification shrine → w Exit. Climb the parish stair through a succession of landings and bring the register back to the village.

**Content placement**

Votive side chapel · The altar stands between offset pews with a sightline through to the way home.

```text
...:...
.X.:.X.
.!.:...
.:::::.
...:.!.
.X.:.X.
...:...
```

X is solid cover, : is the approach inlay, ! is an amber warning lamp, and the middle tile holds the optional prize.

- Votive altar appears on travel lines. Players press Use for lift fuel and a brief burst of speed while travelling.
- Rune flame appears on travel lines. Players pulse on the bright beat for lift fuel and a brief burst of speed while travelling.
- Ward sigil appears on travel lines. Players aim and pulse for lift fuel and a brief burst of speed while travelling.
- Relic drop is staged in a widened court. Use to recover the unmarked offering. The final sequence revisits this action after the shelter.
- Identification shrine is staged in a widened court. Tune the shrine to the relic's mark and pulse. The final sequence revisits this action after the shelter.
- Ward seal is staged in a widened court. Pulse the ward when the revealed signet glows. The final sequence revisits this action after the shelter.
- Unmarked votive sits between two entrances in the cover shown above. The orbiting amber lamp shows where contact will repel the player. Completing the indicated field skill awards a shared ward and opens the return line at the entrance.
- Warm vestry sits three tiles from the first finale anchor. It refills both seats and gives the party control over when to begin.

**Verification and playtest**

Planned intensity **2 → 5 → 7 → 4 → 2 → 9 → 2**. Built mechanical intensity **5 → 6 → 8 → 6 → 2 → 9 → 2**.

The guided path crossed **54 rooms and 1,509 tiles**. Optional room content was **33%** (24 of 72). The five sectors contained **6 independent normal-corridor cycles**. Every authored room has a reachable field reward within **8 tiles** of its centre. A reward is available in every room. The longest guided stretch outside the sensing radius of a field signal or compulsory destination was **2 tiles**.

| Player | Minutes | Marks answered | Wards found | Return rides | Longest quiet stretch | Most room entries between rewards | Climax begins |
|---|---:|---:|---:|---:|---:|---:|---:|
| Solo | 20.60 | 194 | 0 | 0 | 18.3 s | 2 | 77.9% |
| Split | 20.60 | 194 | 0 | 0 | 18.3 s | 2 | 77.9% |
| Rush | 25.38 | 8 | 0 | 0 | 105.7 s | 7 | 80.6% |
| Explore | 25.25 | 230 | 5 | 5 | 18.3 s | 2 | 82.1% |
| Cautious | 21.58 | 200 | 1 | 1 | 18.3 s | 2 | 79.0% |

**Rush.** Passing the signals left this player up to 106 seconds between successes. The field check catches a missed skill before extraction. Answering Votive altar in transit avoids a later detour.
**Explore.** Finding all five instances of Unmarked votive and riding home added 4.6 minutes over the guided run. The mint home marker makes the destination legible before committing.
**Cautious.** The cautious player took the ward loop in the third sector, stunned nearby threats and finished in 21.58 minutes. Warm vestry gives the player a safe place to line up the final approach.
**Revision.** The old run let players pass long stretches with a single movement input. The side room added in this revision is Votive side chapel. The altar stands between offset pews with a sightline through to the way home. This door now teaches Votive altar, develops Rune flame and asks for all three field responses before the final demonstration. The five new ward rooms connect from both sides. The quickest of these observed policies still had to finish the same three final encounters.

## 13 · Boiler Choir

Doom · 1993 · original

Read furnace colours and keys while moving across an arena. A key carried across the firing line opens the route back through it. Aim for the rhythm of moving between safe islands.

Pulse the bellows or skim past. Investigate the brass offcut or follow the shaped socket. Take the blast door or return through the grates.

**Beats and graph**

Cold grate (2) → Red boiler (5) → Mixing room (7) → Valve gallery (4) → Cold boiler jacket (2) → Choir loft (9) → A choirbook margin says the lights once stayed on for somebody who was late (2).

Normal edges follow the spine, side passages and ward loop below. The first tool is the key to the chamber shutter. The mint return is a two-way shortcut earned in V. Sector exits are one-way after the required tools, transmission where present, and field practice. The final exit also requires three signature actions after shelter.

| Sector | Spine | Side passages | Ward loop | Route stations | Rooms / tiles | Optional rooms | Cycles | Dead ends and contents |
|---|---|---|---|---|---:|---:|---:|---|
| Cold grate | `pkfabghmrw` | `klm qrh` | `bcg` | `km` | 9 / 263 | 7/13 | 3 | p arrival landmark, q fuel signal, w extraction |
| Red boiler | `edchglqrsx` | `ihm lkp` | `inh` | `hs` | 10 / 309 | 7/15 | 1 | e arrival landmark, m fuel signal, p fuel signal, x extraction |
| Mixing room | `uvqlmnidej` | `qrs hj` | `vwq` | `qn` | 10 / 267 | 5/14 | 1 | h fuel signal, s fuel signal, u arrival landmark |
| Valve gallery | `afklqvwxsn` | `ghmno lmn` | `him` | `ln` | 11 / 265 | 8/15 | 2 | a arrival landmark, g fuel signal, o fuel signal |
| Choir loft | `ytojihmlqw` | `onm hcb` | `jei` | `om` | 11 / 409 | 4/14 | 2 | b fuel signal, w extraction, y arrival landmark |

- **Cold grate** · Start p. Landmark courts kfagmw. Key Bellows gun at k opens the chamber shutter. Fight or aim rooms k Bellows gallery, m Crossfire grate. Puzzle or traversal rooms Boiler key. Hinted reward c. Shortcut c ↔ p, opened at c. Lock and exit w.
- **Red boiler** · Start e. Landmark courts echlrx. Key Boiler key at d → c opens the chamber shutter. Fight or aim rooms s Crossfire grate. Puzzle or traversal rooms h Shape key. Hinted reward n. Shortcut n ↔ e, opened at n. Lock and exit x.
- **Mixing room** · Start u. Landmark courts uvlmij. Key Furnace choir at v → q opens the chamber shutter. Fight or aim rooms q Crossfire grate, n Bellows gallery. Puzzle or traversal rooms Boiler key. Hinted reward w. Shortcut w ↔ u, opened at w. Lock and exit j.
- **Valve gallery** · Start a. Landmark courts afklwn. Key Boiler key at f → k opens the chamber shutter. Fight or aim rooms l Bellows gallery. Puzzle or traversal rooms n Shape key. Hinted reward i. Shortcut i ↔ a, opened at i. Lock and exit n.
- **Choir loft** · Start y. Landmark courts ytohmw. Key Furnace choir at t → o opens the chamber shutter. Fight or aim rooms m Bellows gallery, h Crossfire grate. Puzzle or traversal rooms o Shape key. Hinted reward e. Shortcut e ↔ y, opened at e. Lock and exit w. Rest at Cold boiler jacket.

**Layout and critical walk**

```text
    Sector 1          Sector 2          Sector 3          Sector 4          Sector 5
a* b* cV                c* d* eS             d* e*    aS                   b. c.    eV
f. g. h.             g* h* i.             h. i* jE    f* g. h. iV             h* i* j*
k* l. m*          k. l* m. nV          l* m. n*       k* l* m. nE o.       l* m* n. o*
pS q. r.          p. q. r. s*          q* r. s.          q.    s.          q*       t*
      wE                   xE       uS v* wV             v* w* x.             wE    yS
```

- **Cold grate** · k Bellows gun → k Bellows gallery → m Crossfire grate → a Boiler key → b Boiler key finish → w Exit. Three firing lanes feed the grate room. Pick up the boiler key in the side gallery.
- **Red boiler** · d Boiler key → c Boiler key finish → h Shape key → s Crossfire grate → h Furnace choir → g Furnace choir finish → l Bellows gun → x Exit. The red boiler splits the key route around its jacket. Open the chamber from its service side.
- **Mixing room** · v Furnace choir → q Furnace choir finish → q Crossfire grate → n Bellows gallery → l Bellows gun → n Boiler key → i Boiler key finish → d Recover dispatch → e Answer dispatch → j Exit. Criss-cross the mixing room between key alcoves and exposed furnace pipes.
- **Valve gallery** · f Boiler key → k Boiler key finish → l Bellows gallery → n Shape key → l Bellows gun → v Furnace choir → w Furnace choir finish → n Exit. A narrow valve gallery unfolds into a horseshoe arena. The bellows can reach across the gap.
- **Choir loft** · t Furnace choir → o Furnace choir finish → j Boiler key → i Boiler key finish → h Bellows gun → l Recover dispatch → q Answer dispatch → (88,51) Cold boiler jacket → o Shape key → m Bellows gallery → h Crossfire grate → w Exit. The choir loft overlooks the three furnace mouths. Prime the pipe before answering the missing note.

**Content placement**

Furnace jacket · Paired baffles create two safe islands facing the same bellows mouth.

```text
..:.:..
.!:.XX.
..:....
..::::.
....:..
.XX.:!.
..:.:..
```

X is solid cover, : is the approach inlay, ! is an amber warning lamp, and the middle tile holds the optional prize.

- Bellows mouth appears on travel lines. Players aim and pulse for lift fuel and a brief burst of speed while travelling.
- Grate valve appears on travel lines. Players pulse on the bright beat for lift fuel and a brief burst of speed while travelling.
- Ash channel appears on travel lines. Players cross at ground height for lift fuel and a brief burst of speed while travelling.
- Bellows gallery is staged in a widened court. Pump three shots into the moving furnace mouths. The final sequence revisits this action after the shelter.
- Shape key is staged in a widened court. Take the matching key to its boiler socket and use. The final sequence revisits this action after the shelter.
- Crossfire grate is staged in a widened court. Pulse as the two flame valves line up. The final sequence revisits this action after the shelter.
- Brass offcut sits between two entrances in the cover shown above. The orbiting amber lamp shows where contact will repel the player. Completing the indicated field skill awards a shared ward and opens the return line at the entrance.
- Cold boiler jacket sits three tiles from the first finale anchor. It refills both seats and gives the party control over when to begin.

**Verification and playtest**

Planned intensity **2 → 5 → 7 → 4 → 2 → 9 → 2**. Built mechanical intensity **6 → 6 → 7 → 5 → 2 → 9 → 2**.

The guided path crossed **51 rooms and 1,513 tiles**. Optional room content was **44%** (31 of 71). The five sectors contained **9 independent normal-corridor cycles**. Every authored room has a reachable field reward within **9 tiles** of its centre. A reward is available in every room. The longest guided stretch outside the sensing radius of a field signal or compulsory destination was **3 tiles**.

| Player | Minutes | Marks answered | Wards found | Return rides | Longest quiet stretch | Most room entries between rewards | Climax begins |
|---|---:|---:|---:|---:|---:|---:|---:|
| Solo | 20.62 | 205 | 0 | 0 | 17.0 s | 2 | 89.5% |
| Split | 20.62 | 205 | 0 | 0 | 17.1 s | 2 | 89.5% |
| Rush | 23.40 | 64 | 0 | 0 | 59.8 s | 2 | 90.0% |
| Explore | 26.65 | 259 | 5 | 5 | 19.1 s | 2 | 91.8% |
| Cautious | 21.20 | 211 | 1 | 1 | 17.0 s | 2 | 89.7% |

**Rush.** Passing the signals left this player up to 60 seconds between successes. The field check catches a missed skill before extraction. Answering Bellows mouth in transit avoids a later detour.
**Explore.** Finding all five instances of Brass offcut and riding home added 6.0 minutes over the guided run. The mint home marker makes the destination legible before committing.
**Cautious.** The cautious player took the ward loop in the third sector, stunned nearby threats and finished in 21.20 minutes. Cold boiler jacket gives the player a safe place to line up the final approach.
**Revision.** The old run let players pass long stretches with a single movement input. The side room added in this revision is Furnace jacket. Paired baffles create two safe islands facing the same bellows mouth. This door now teaches Bellows mouth, develops Grate valve and asks for all three field responses before the final demonstration. The five new ward rooms connect from both sides. The quickest of these observed policies still had to finish the same three final encounters.

## 14 · Orchard of Small Wishes

Secret of Mana · 1993 · SNES original

Link a charged cut, a ribbon crossing and a seed's answering note. A ribbon first cut as an obstacle becomes the way across a gap. Aim for an orchard that responds to the player's rhythm.

Answer a bud on the beat or keep travelling. Spend lift on the grafted seed or save it for a high peg. Swing to the far bough or return along the roots.

**Beats and graph**

Nursery (2) → Ribbon boughs (5) → Root choir (7) → Seed grove (4) → Root hollow (2) → Heartwood (9) → A small seed bell remembers a friend humming between matches (2).

Normal edges follow the spine, side passages and ward loop below. The first tool is the key to the chamber shutter. The mint return is a two-way shortcut earned in V. Sector exits are one-way after the required tools, transmission where present, and field practice. The final exit also requires three signature actions after shelter.

| Sector | Spine | Side passages | Ward loop | Route stations | Rooms / tiles | Optional rooms | Cycles | Dead ends and contents |
|---|---|---|---|---|---:|---:|---:|---|
| Nursery | `uvqlghmnsy` | `mqv hin` | `gbh` | `qm` | 10 / 178 | 2/12 | 3 | u arrival landmark, y extraction |
| Ribbon boughs | `fbcioswrlk` | `hms qlm` | `lgk` | `is` | 10 / 283 | 4/14 | 2 | f arrival landmark, h fuel signal, q fuel signal |
| Root choir | `agkqmsoieh` | `gch lmn` | `gfk` | `gm` | 9 / 309 | 6/14 | 2 | a arrival landmark, l fuel signal, n fuel signal |
| Seed grove | `wvpkgbdjnm` | `klm ihd` | `kfl` | `pn` | 11 / 264 | 4/14 | 2 | i fuel signal, w arrival landmark |
| Heartwood | `ejosrmlgch` | `nmr chg` | `cdh` | `mh` | 11 / 319 | 1/12 | 2 | e arrival landmark, n tool / field reward |

- **Nursery** · Start u. Landmark courts uvghmy. Key Pruning blade at v → q opens the chamber shutter. Fight or aim rooms m Elemental seed bell. Puzzle or traversal rooms q Charged pruning cut. Hinted reward b. Shortcut b ↔ u, opened at b. Lock and exit y.
- **Ribbon boughs** · Start f. Landmark courts fbiosw. Key Ribbon peg at b → c opens the chamber shutter. Fight or aim rooms use the active tool sentry. Puzzle or traversal rooms i Ribbon peg, s Charged pruning cut. Hinted reward g. Shortcut g ↔ f, opened at g. Lock and exit k.
- **Root choir** · Start a. Landmark courts agqmso. Key Elemental notes at g → k opens the chamber shutter. Fight or aim rooms g Elemental seed bell. Puzzle or traversal rooms m Ribbon peg. Hinted reward f. Shortcut f ↔ a, opened at f. Lock and exit h.
- **Seed grove** · Start w. Landmark courts vpkgbn. Key Elemental notes at v → p opens the chamber shutter. Fight or aim rooms use the active tool sentry. Puzzle or traversal rooms p Charged pruning cut, n Ribbon peg. Hinted reward f. Shortcut f ↔ w, opened at f. Lock and exit m.
- **Heartwood** · Start e. Landmark courts ejosmg. Key Ribbon peg at j → o opens the chamber shutter. Fight or aim rooms n Elemental seed bell. Puzzle or traversal rooms m Charged pruning cut, h Ribbon peg. Hinted reward d. Shortcut d ↔ e, opened at d. Lock and exit h. Rest at Root hollow.

**Layout and critical walk**

```text
    Sector 1          Sector 2          Sector 3          Sector 4          Sector 5
   bV                b* c*          aS    c.    e*       b*    d*             c* dV eS
   g* h* i.       fS gV h. i*       fV g* hE i*       fV g* h. i. j*       g* hE    j*
   l* m* n*       kE l* m.    o*    k* l. m* n. o.    k* l. mE n*          l* m* n* o*
   q*    s*          q. r* s*          q*    s.       p*                      r* s*
uS v*       yE          w*                               v* wS
```

- **Nursery** · v Pruning blade → q Pruning blade finish → q Charged pruning cut → m Elemental seed bell → l Ribbon peg → g Ribbon peg finish → h Elemental notes → m Elemental notes finish → n Recover dispatch → s Answer dispatch → y Exit. Prune a way out of the clustered nursery beds. A ribbon spans the fork in the orchard path.
- **Ribbon boughs** · b Ribbon peg → c Ribbon peg finish → i Ribbon peg → s Charged pruning cut → i Pruning blade → o Pruning blade finish → s Elemental notes → w Elemental notes finish → r Recover dispatch → l Answer dispatch → k Exit. A fan of boughs hangs over the clearing. Use the marked pegs to cross between branches.
- **Root choir** · g Elemental notes → k Elemental notes finish → g Elemental seed bell → m Ribbon peg → q Pruning blade → m Pruning blade finish → i Recover dispatch → e Answer dispatch → h Exit. Root paths converge on a hollow singing tree. Try the elemental note at the heart of the ring.
- **Seed grove** · v Elemental notes → p Elemental notes finish → p Charged pruning cut → n Ribbon peg → k Pruning blade → g Pruning blade finish → b Ribbon peg → d Ribbon peg finish → j Recover dispatch → n Answer dispatch → m Exit. Seed beds lie in a crescent around the gully. Carry their song along the ribbon crossing.
- **Heartwood** · j Ribbon peg → o Ribbon peg finish → s Elemental notes → r Elemental notes finish → m Pruning blade → l Pruning blade finish → g Recover dispatch → c Answer dispatch → (34,37) Root hollow → m Charged pruning cut → h Ribbon peg → n Elemental seed bell → h Exit. Work inward from the bark to the heartwood along the forks of a single immense tree.

**Content placement**

Grafting circle · The curling root path exposes the graft a little at a time before opening at the peg.

```text
..:::..
.X:.:!.
..:....
..::::.
.X...:.
.!X..:.
...:::.
```

X is solid cover, : is the approach inlay, ! is an amber warning lamp, and the middle tile holds the optional prize.

- Pruning knot appears on travel lines. Players charge and release on the plate for lift fuel and a brief burst of speed while travelling.
- Ribbon peg appears on travel lines. Players lift through the crown for lift fuel and a brief burst of speed while travelling.
- Answering bud appears on travel lines. Players pulse on the bright beat for lift fuel and a brief burst of speed while travelling.
- Charged pruning cut is staged in a widened court. Hold the blade on the knotted ribbon until it parts. The final sequence revisits this action after the shelter.
- Ribbon peg is staged in a widened court. Lift through the high peg to cross the bough. The final sequence revisits this action after the shelter.
- Elemental seed bell is staged in a widened court. Pulse the seed bell on its bright answering beat. The final sequence revisits this action after the shelter.
- Grafted seed sits between two entrances in the cover shown above. The orbiting amber lamp shows where contact will repel the player. Completing the indicated field skill awards a shared ward and opens the return line at the entrance.
- Root hollow sits three tiles from the first finale anchor. It refills both seats and gives the party control over when to begin.

**Verification and playtest**

Planned intensity **2 → 5 → 7 → 4 → 2 → 9 → 2**. Built mechanical intensity **6 → 5 → 7 → 5 → 2 → 9 → 2**.

The guided path crossed **51 rooms and 1,353 tiles**. Optional room content was **26%** (17 of 66). The five sectors contained **11 independent normal-corridor cycles**. Every authored room has a reachable field reward within **7 tiles** of its centre. A reward is available in every room. The longest guided stretch outside the sensing radius of a field signal or compulsory destination was **2 tiles**.

| Player | Minutes | Marks answered | Wards found | Return rides | Longest quiet stretch | Most room entries between rewards | Climax begins |
|---|---:|---:|---:|---:|---:|---:|---:|
| Solo | 18.58 | 185 | 0 | 0 | 17.6 s | 2 | 90.1% |
| Split | 18.60 | 183 | 0 | 0 | 17.6 s | 2 | 90.1% |
| Rush | 20.33 | 11 | 0 | 0 | 54.3 s | 3 | 90.3% |
| Explore | 24.33 | 239 | 5 | 5 | 21.6 s | 2 | 92.5% |
| Cautious | 18.75 | 187 | 1 | 1 | 17.6 s | 2 | 90.2% |

**Rush.** Passing the signals left this player up to 54 seconds between successes. The field check catches a missed skill before extraction. Answering Pruning knot in transit avoids a later detour.
**Explore.** Finding all five instances of Grafted seed and riding home added 5.8 minutes over the guided run. The mint home marker makes the destination legible before committing.
**Cautious.** The cautious player took the ward loop in the third sector, stunned nearby threats and finished in 18.75 minutes. Root hollow gives the player a safe place to line up the final approach.
**Revision.** The old run let players pass long stretches with a single movement input. The side room added in this revision is Grafting circle. The curling root path exposes the graft a little at a time before opening at the peg. This door now teaches Pruning knot, develops Ribbon peg and asks for all three field responses before the final demonstration. The five new ward rooms connect from both sides. The quickest of these observed policies still had to finish the same three final encounters.

## 15 · Footprints at Dusk

X-COM: UFO Defense / UFO: Enemy Unknown · 1994 · MicroProse original

Search a dark field by revealing one safe position at a time. A glimpse from cover matters more than rushing the next footprint. Aim for suspense that can be managed by watching.

Pulse from a blind or approach the next light. Cross to the sample plate or keep the surveyor close. Take the return passage together or sweep back across the field.

**Beats and graph**

Landing field (2) → Lantern barn (5) → Orchard tracks (7) → Mimic nest (4) → Lit barn (2) → Survey pickup (9) → The surveyor’s notebook records a successful rescue after the official deadline (2).

Normal edges follow the spine, side passages and ward loop below. The first tool is the key to the chamber shutter. The mint return is a two-way shortcut earned in V. Sector exits are one-way after the required tools, transmission where present, and field practice. The final exit also requires three signature actions after shelter.

| Sector | Spine | Side passages | Ward loop | Route stations | Rooms / tiles | Optional rooms | Cycles | Dead ends and contents |
|---|---|---|---|---|---:|---:|---:|---|
| Landing field | `upklgbchij` | `qrm lmh pq` | `cdh` | `lh` | 11 / 225 | 5/14 | 3 | j extraction, u arrival landmark |
| Lantern barn | `ejonihmrqp` | `nmlo rsx` | `otn` | `nr` | 10 / 227 | 5/14 | 3 | e arrival landmark, p extraction, x fuel signal |
| Orchard tracks | `afkqrsnidc` | `lgh qpu fg` | `noi` | `kn` | 10 / 266 | 9/16 | 1 | a arrival landmark, c extraction, h fuel signal, l fuel signal, u fuel signal |
| Mimic nest | `ytsxwrmgfb` | `rql hin sn` | `ijn` | `sm` | 9 / 282 | 8/16 | 1 | b extraction, h fuel signal, l fuel signal, y arrival landmark |
| Survey pickup | `kpvwxsnhcd` | `qrl mno kl` | `stn` | `vh` | 11 / 503 | 5/16 | 1 | d extraction, m tool / field reward, o fuel signal, q fuel signal |

- **Landing field** · Start u. Landmark courts upklbj. Key Survey flare at p → k opens the chamber shutter. Fight or aim rooms h Reaction reserve. Puzzle or traversal rooms l Survey flare. Hinted reward d. Shortcut d ↔ u, opened at d. Lock and exit j.
- **Lantern barn** · Start e. Landmark courts eonhrp. Key Reaction flash at j → o opens the chamber shutter. Fight or aim rooms n Reaction reserve, r Recovery capsule. Puzzle or traversal rooms Lost surveyor. Hinted reward t. Shortcut t ↔ e, opened at t. Lock and exit p.
- **Orchard tracks** · Start a. Landmark courts afkqni. Key Survey flare at f → k opens the chamber shutter. Fight or aim rooms n Reaction reserve. Puzzle or traversal rooms k Survey flare. Hinted reward o. Shortcut o ↔ a, opened at o. Lock and exit c.
- **Mimic nest** · Start y. Landmark courts ysxrmb. Key Lost surveyor at t → s opens the chamber shutter. Fight or aim rooms s Reaction reserve, m Recovery capsule. Puzzle or traversal rooms r Survey flare. Hinted reward j. Shortcut j ↔ y, opened at j. Lock and exit b.
- **Survey pickup** · Start k. Landmark courts kpvxnd. Key Lost surveyor at p → v opens the chamber shutter. Fight or aim rooms h Reaction reserve, m Recovery capsule. Puzzle or traversal rooms v Survey flare. Hinted reward t. Shortcut t ↔ k, opened at t. Lock and exit d. Rest at Lit barn.

**Layout and critical walk**

```text
    Sector 1          Sector 2          Sector 3          Sector 4          Sector 5
   b* c* dV                   eS    aS    cE d.          bE                   c* dE
   g* h* i. jE          h* i* j*    f* g. h. i.       f. g. h. i. jV          h*
k* l* m.             l. m* n* o*    k* l.    n* oV       l. m* n.       kS l. m* n* o.
p* q. r.          pE q. r* s. tV    p. q* r* s.          q. r* s* t*    p* q. r. s* tV
uS                         x.       u.                      w* x* yS       v* w* x*
```

- **Landing field** · p Survey flare → k Survey flare finish → l Survey flare → h Reaction reserve → l Reaction flash → g Reaction flash finish → b Lost surveyor → c Lost surveyor finish → j Exit. Sweep the hedges outward from the landing field. A flare at the blind turn reveals the track.
- **Lantern barn** · j Reaction flash → o Reaction flash finish → n Reaction reserve → r Recovery capsule → n Survey flare → i Survey flare finish → h Lost surveyor → m Lost surveyor finish → p Exit. A lantern barn has an open threshing floor and dark stalls. Cover its exits before collecting the witness.
- **Orchard tracks** · f Survey flare → k Survey flare finish → k Survey flare → n Reaction reserve → q Reaction flash → r Reaction flash finish → c Exit. Follow tracks across alternating orchard rows. The reaction blind looks back across the last row.
- **Mimic nest** · t Lost surveyor → s Lost surveyor finish → s Reaction reserve → m Recovery capsule → r Survey flare → x Reaction flash → w Reaction flash finish → r Survey flare → m Survey flare finish → b Exit. The nest is concealed behind two banks of crooked hedges. Keep a reaction flash ready.
- **Survey pickup** · p Lost surveyor → v Lost surveyor finish → w Survey flare → x Survey flare finish → s Reaction flash → n Reaction flash finish → h Recover dispatch → c Answer dispatch → (25,79) Lit barn → v Survey flare → h Reaction reserve → m Recovery capsule → d Exit. Escort the surveyor across the recovery perimeter. The shuttle ramp opens onto a lit clearing.

**Content placement**

Blind in the orchard · Two crooked orchard rows give the surveyor cover while keeping the sample plate in sight.

```text
..:....
.X:..!.
..:.X..
..:::..
..X.:..
.!..:X.
....:..
```

X is solid cover, : is the approach inlay, ! is an amber warning lamp, and the middle tile holds the optional prize.

- Mimic flash appears on travel lines. Players pulse on the bright beat for lift fuel and a brief burst of speed while travelling.
- Survey blind appears on travel lines. Players press Use for lift fuel and a brief burst of speed while travelling.
- Track flare appears on travel lines. Players aim and pulse for lift fuel and a brief burst of speed while travelling.
- Survey flare is staged in a widened court. Use a flare at the last footprint to mark the search zone. The final sequence revisits this action after the shelter.
- Reaction reserve is staged in a widened court. Pulse as the mimic leaves cover on the green beat. The final sequence revisits this action after the shelter.
- Recovery capsule is staged in a widened court. Hold the stun lamp on the capsule to secure the live specimen. The final sequence revisits this action after the shelter.
- Sample plate sits between two entrances in the cover shown above. The orbiting amber lamp shows where contact will repel the player. Completing the indicated field skill awards a shared ward and opens the return line at the entrance.
- Lit barn sits three tiles from the first finale anchor. It refills both seats and gives the party control over when to begin.

**Verification and playtest**

Planned intensity **2 → 5 → 7 → 4 → 2 → 9 → 2**. Built mechanical intensity **6 → 6 → 7 → 7 → 2 → 9 → 2**.

The guided path crossed **51 rooms and 1,503 tiles**. Optional room content was **42%** (32 of 76). The five sectors contained **9 independent normal-corridor cycles**. Every authored room has a reachable field reward within **8 tiles** of its centre. A reward is available in every room. The longest guided stretch outside the sensing radius of a field signal or compulsory destination was **2 tiles**.

| Player | Minutes | Marks answered | Wards found | Return rides | Longest quiet stretch | Most room entries between rewards | Climax begins |
|---|---:|---:|---:|---:|---:|---:|---:|
| Solo | 20.37 | 194 | 0 | 0 | 15.5 s | 2 | 85.7% |
| Split | 20.37 | 194 | 0 | 0 | 15.5 s | 2 | 85.7% |
| Rush | 23.01 | 8 | 0 | 0 | 106.4 s | 2 | 86.2% |
| Explore | 27.27 | 248 | 5 | 5 | 18.0 s | 2 | 89.3% |
| Cautious | 22.18 | 207 | 1 | 1 | 15.5 s | 2 | 86.8% |

**Rush.** Passing the signals left this player up to 106 seconds between successes. The field check catches a missed skill before extraction. Answering Mimic flash in transit avoids a later detour.
**Explore.** Finding all five instances of Sample plate and riding home added 6.9 minutes over the guided run. The mint home marker makes the destination legible before committing.
**Cautious.** The cautious player took the ward loop in the third sector, stunned nearby threats and finished in 22.18 minutes. Lit barn gives the player a safe place to line up the final approach.
**Revision.** The old run let players pass long stretches with a single movement input. The side room added in this revision is Blind in the orchard. Two crooked orchard rows give the surveyor cover while keeping the sample plate in sight. This door now teaches Mimic flash, develops Survey blind and asks for all three field responses before the final demonstration. The five new ward rooms connect from both sides. The quickest of these observed policies still had to finish the same three final encounters.

## 16 · The Nailwright’s Stair

Quake · 1996 · first game

Connect a vertical rivet gallery with paired runes. The high ledge you fired at becomes the landing for your return. Aim for the confidence of recognising a room across a vertical drop.

Shoot the distant stop or climb before firing. Jet for the maker's die or approach the next rune. Use a slipgate or climb the stair again.

**Beats and graph**

Lower foundry (2) → Rune crossing (5) → Ratchet gallery (7) → Upper shaft (4) → Lower landing (2) → Bell room (9) → A foundry nail fixes a torn corner of the original invitation (2).

Normal edges follow the spine, side passages and ward loop below. The first tool is the key to the chamber shutter. The mint return is a two-way shortcut earned in V. Sector exits are one-way after the required tools, transmission where present, and field practice. The final exit also requires three signature actions after shelter.

| Sector | Spine | Side passages | Ward loop | Route stations | Rooms / tiles | Optional rooms | Cycles | Dead ends and contents |
|---|---|---|---|---|---:|---:|---:|---|
| Lower foundry | `upkfbchmns` | `klm nrw` | `bgc` | `km` | 11 / 203 | 5/14 | 2 | s extraction, u arrival landmark, w fuel signal |
| Rune crossing | `ejidchglqv` | `imr ghl` | `gfl` | `iv` | 10 / 297 | 6/13 | 2 | e arrival landmark, r fuel signal, v extraction |
| Ratchet gallery | `abglqrmnot` | `chm qrsv` | `nio` | `gn` | 10 / 237 | 6/15 | 1 | a arrival landmark, c fuel signal, t extraction, v fuel signal |
| Upper shaft | `wvpklghino` | `qrm hdc wr` | `ijn` | `lh` | 9 / 223 | 9/16 | 1 | c fuel signal, m fuel signal, o extraction, q fuel signal |
| Bell room | `mrxytojdch` | `mlgb ors` | `onj` | `mh` | 11 / 569 | 6/15 | 2 | b fuel signal, h extraction, s fuel signal |

- **Lower foundry** · Start u. Landmark courts ukfbms. Key Rivet caster at p → k opens the chamber shutter. Fight or aim rooms k Ratchet stops. Puzzle or traversal rooms m Slipgate rune. Hinted reward g. Shortcut g ↔ u, opened at g. Lock and exit s.
- **Rune crossing** · Start e. Landmark courts ejdhlv. Key Paired gate at j → i opens the chamber shutter. Fight or aim rooms use the active tool sentry. Puzzle or traversal rooms i Slipgate rune, v Impulse lip. Hinted reward f. Shortcut f ↔ e, opened at f. Lock and exit v.
- **Ratchet gallery** · Start a. Landmark courts abgqmt. Key Rivet caster at b → g opens the chamber shutter. Fight or aim rooms g Ratchet stops. Puzzle or traversal rooms n Impulse lip. Hinted reward i. Shortcut i ↔ a, opened at i. Lock and exit t.
- **Upper shaft** · Start w. Landmark courts wvklho. Key Impulse pad at v opens the chamber shutter. Fight or aim rooms h Ratchet stops. Puzzle or traversal rooms l Impulse lip. Hinted reward j. Shortcut j ↔ w, opened at j. Lock and exit o.
- **Bell room** · Start m. Landmark courts mrxyoh. Key Impulse pad at r opens the chamber shutter. Fight or aim rooms h Ratchet stops. Puzzle or traversal rooms m Slipgate rune, r Impulse lip. Hinted reward n. Shortcut n ↔ m, opened at n. Lock and exit h. Rest at Lower landing.

**Layout and critical walk**

```text
    Sector 1          Sector 2          Sector 3          Sector 4          Sector 5
   b. c*                c* d* eS    aS b* c.                c. d.          b. c* d*
f* gV h*          fV g. h* i* j*       g* h. iV          g* h* i. jV       g. hE    j*
k* l. m* n*          l. m.             l* m. n* o*    k* l* m. n. oE       l. mS nV o*
p*    r. sE          q. r.             q* r* s. tE    p. q. r.                r* s. t*
uS    w.             vE                v.                v* wS                   x. y*
```

- **Lower foundry** · p Rivet caster → k Rivet caster finish → k Ratchet stops → m Slipgate rune → f Impulse pad → c Paired gate → h Paired gate finish → m Recover dispatch → n Answer dispatch → s Exit. A switchback climbs from the foundry floor to the shaft. Pin the first turning stop from below.
- **Rune crossing** · j Paired gate → i Paired gate finish → i Slipgate rune → v Impulse lip → d Rivet caster → c Rivet caster finish → h Impulse pad → v Exit. Two rune balconies hang on opposite sides of the stair. Their paired mouths shorten the crossing.
- **Ratchet gallery** · b Rivet caster → g Rivet caster finish → g Ratchet stops → n Impulse lip → l Paired gate → q Paired gate finish → r Impulse pad → n Recover dispatch → o Answer dispatch → t Exit. Ratchets sit in alternating wall recesses. Sight a rivet across the gear well from each landing.
- **Upper shaft** · v Impulse pad → l Impulse lip → h Ratchet stops → k Rivet caster → l Rivet caster finish → g Paired gate → h Paired gate finish → o Exit. The stair wraps the open shaft. Reach the impulse ledge above the old casting floor.
- **Bell room** · r Impulse pad → y Paired gate → t Paired gate finish → o Rivet caster → j Rivet caster finish → d Recover dispatch → c Answer dispatch → (36,39) Lower landing → m Slipgate rune → h Ratchet stops → r Impulse lip → h Exit. Descend by the far side of the bell chamber. The final rune folds the long stair back on itself.

**Content placement**

Rivet die gallery · A diagonal break between paired buttresses lines up the sight from the lower landing.

```text
.....:.
.XX..:.
.!..::.
..:::..
.::..!.
.:..XX.
.:.....
```

X is solid cover, : is the approach inlay, ! is an amber warning lamp, and the middle tile holds the optional prize.

- Rivet stop appears on travel lines. Players aim and pulse for lift fuel and a brief burst of speed while travelling.
- Impulse lip appears on travel lines. Players lift through the crown for lift fuel and a brief burst of speed while travelling.
- Rune socket appears on travel lines. Players press Use for lift fuel and a brief burst of speed while travelling.
- Ratchet stops is staged in a widened court. Shoot a rivet into each of the three moving ratchets. The final sequence revisits this action after the shelter.
- Impulse lip is staged in a widened court. Lift through the ratchet ring above the shaft. The final sequence revisits this action after the shelter.
- Slipgate rune is staged in a widened court. Use to seat the matching rune in the paired gate. The final sequence revisits this action after the shelter.
- Maker's die sits between two entrances in the cover shown above. The orbiting amber lamp shows where contact will repel the player. Completing the indicated field skill awards a shared ward and opens the return line at the entrance.
- Lower landing sits three tiles from the first finale anchor. It refills both seats and gives the party control over when to begin.

**Verification and playtest**

Planned intensity **2 → 5 → 7 → 4 → 2 → 9 → 2**. Built mechanical intensity **5 → 5 → 6 → 5 → 2 → 8 → 2**.

The guided path crossed **51 rooms and 1,529 tiles**. Optional room content was **44%** (32 of 73). The five sectors contained **8 independent normal-corridor cycles**. Every authored room has a reachable field reward within **9 tiles** of its centre. A reward is available in every room. The longest guided stretch outside the sensing radius of a field signal or compulsory destination was **1 tiles**.

| Player | Minutes | Marks answered | Wards found | Return rides | Longest quiet stretch | Most room entries between rewards | Climax begins |
|---|---:|---:|---:|---:|---:|---:|---:|
| Solo | 20.93 | 160 | 0 | 0 | 25.8 s | 2 | 78.7% |
| Split | 20.93 | 160 | 0 | 0 | 25.8 s | 2 | 78.7% |
| Rush | 23.40 | 11 | 0 | 0 | 99.9 s | 7 | 79.5% |
| Explore | 27.13 | 197 | 5 | 5 | 26.9 s | 2 | 83.6% |
| Cautious | 22.43 | 168 | 1 | 1 | 25.8 s | 2 | 80.1% |

**Rush.** Passing the signals left this player up to 100 seconds between successes. The field check catches a missed skill before extraction. Answering Rivet stop in transit avoids a later detour.
**Explore.** Finding all five instances of Maker's die and riding home added 6.2 minutes over the guided run. The mint home marker makes the destination legible before committing.
**Cautious.** The cautious player took the ward loop in the third sector, stunned nearby threats and finished in 22.43 minutes. Lower landing gives the player a safe place to line up the final approach.
**Revision.** The old run let players pass long stretches with a single movement input. The side room added in this revision is Rivet die gallery. A diagonal break between paired buttresses lines up the sight from the lower landing. This door now teaches Rivet stop, develops Impulse lip and asks for all three field responses before the final demonstration. The five new ward rooms connect from both sides. The quickest of these observed policies still had to finish the same three final encounters.

## 17 · Apprentice of the Aurora

Jedi Knight: Dark Forces II · 1997 · original Jedi Knight

Alternate a timed light parry with focus movement. The parry sightline ends at a counterweight the apprentice must pull. Aim for calm precision at height.

Meet the flash or conserve attention for the jump. Reach the prism chip or hold lift for the gallery. Cross on the focus lens or take the grounded approach.

**Beats and graph**

Prism gallery (2) → Ravine bridge (5) → Counterweight shaft (7) → Lens garden (4) → Prism cloister (2) → Split dome (9) → The apprentice remembers the rest of the correction and writes ANYWAY below START (2).

Normal edges follow the spine, side passages and ward loop below. The first tool is the key to the chamber shutter. The mint return is a two-way shortcut earned in V. Sector exits are one-way after the required tools, transmission where present, and field practice. The final exit also requires three signature actions after shelter.

| Sector | Spine | Side passages | Ward loop | Route stations | Rooms / tiles | Optional rooms | Cycles | Dead ends and contents |
|---|---|---|---|---|---:|---:|---:|---|
| Prism gallery | `afghcdinsx` | `lmn gqr` | `dei` | `gn` | 12 / 227 | 5/15 | 1 | a arrival landmark, l fuel signal, r fuel signal, x extraction |
| Ravine bridge | `upkglqrsno` | `mhi qvw gh` | `hci` | `ln` | 10 / 288 | 6/16 | 1 | m fuel signal, o extraction, u arrival landmark, w fuel signal |
| Counterweight shaft | `ejotxwrmhb` | `onm rql` | `rsm` | `om` | 11 / 233 | 4/14 | 2 | b extraction, e arrival landmark, l fuel signal |
| Lens garden | `kfbcinmrwv` | `hmg qrs` | `mlr` | `im` | 10 / 287 | 5/15 | 1 | g fuel signal, h fuel signal, k arrival landmark, q fuel signal, s fuel signal, v extraction |
| Split dome | `ytoichmglp` | `ins bgf` | `idc` | `im` | 11 / 373 | 5/15 | 1 | b fuel signal, f fuel signal, p extraction, s fuel signal, y arrival landmark |

- **Prism gallery** · Start a. Landmark courts afghix. Key Prism staff at f → g opens the chamber shutter. Fight or aim rooms g Prism parry, n Counterweight pull. Puzzle or traversal rooms Lens weight. Hinted reward e. Shortcut e ↔ a, opened at e. Lock and exit x.
- **Ravine bridge** · Start u. Landmark courts upklso. Key Lens weight at p → k opens the chamber shutter. Fight or aim rooms l Counterweight pull. Puzzle or traversal rooms n Focus leap. Hinted reward c. Shortcut c ↔ u, opened at c. Lock and exit o.
- **Counterweight shaft** · Start e. Landmark courts ejoxmh. Key Focus leap at j → o opens the chamber shutter. Fight or aim rooms m Counterweight pull. Puzzle or traversal rooms o Focus leap. Hinted reward s. Shortcut s ↔ e, opened at s. Lock and exit b.
- **Lens garden** · Start k. Landmark courts kfbinw. Key Lens weight at f → b opens the chamber shutter. Fight or aim rooms i Prism parry. Puzzle or traversal rooms m Focus leap. Hinted reward l. Shortcut l ↔ k, opened at l. Lock and exit v.
- **Split dome** · Start y. Landmark courts ytohmp. Key Focus leap at t → o opens the chamber shutter. Fight or aim rooms i Counterweight pull, m Prism parry. Puzzle or traversal rooms h Focus leap. Hinted reward d. Shortcut d ↔ y, opened at d. Lock and exit p. Rest at Prism cloister.

**Layout and critical walk**

```text
    Sector 1          Sector 2          Sector 3          Sector 4          Sector 5
aS    c* d* eV          cV             bE       eS       b* c*             b. c* dV
f* g* h* i*          g* h. i.             h*    j*    f* g. h. i*       f. g* h* i*
   l. m. n*       k* l* m. n* oE       l. m* n. o*    kS lV m* n*          l* m* n. o*
   q. r. s*       p* q* r* s*          q. r* sV t*       q. r* s.       pE       s. t*
         xE       uS v. w.                w* x*          vE w*                      yS
```

- **Prism gallery** · f Prism staff → g Prism staff finish → g Prism parry → n Counterweight pull → h Focus leap → c Focus leap finish → d Lens weight → i Lens weight finish → n Recover dispatch → s Answer dispatch → x Exit. Prism colonnades split the gallery light. The last beam meets a high landing.
- **Ravine bridge** · p Lens weight → k Lens weight finish → l Counterweight pull → n Focus leap → g Prism staff → l Prism staff finish → q Focus leap → r Focus leap finish → s Recover dispatch → n Answer dispatch → o Exit. Approach the ravine along the west shelf and return along the east. Draw the bridge weights across.
- **Counterweight shaft** · j Focus leap → o Focus leap finish → o Focus leap → m Counterweight pull → t Prism staff → x Prism staff finish → w Lens weight → r Lens weight finish → m Recover dispatch → h Answer dispatch → b Exit. The counterweight shaft has offset landings. Leap from the balance arm to the upper latch.
- **Lens garden** · f Lens weight → b Lens weight finish → i Prism parry → m Focus leap → c Focus leap → i Focus leap finish → n Prism staff → m Prism staff finish → r Recover dispatch → w Answer dispatch → v Exit. A sequence of lens islands bends through the garden. Line up the staff with the far mirror.
- **Split dome** · t Focus leap → o Focus leap finish → i Lens weight → c Lens weight finish → h Prism staff → m Prism staff finish → g Recover dispatch → l Answer dispatch → (53,26) Prism cloister → i Counterweight pull → m Prism parry → h Focus leap → p Exit. The split dome has two galleries facing the same prism. Realign its light to release the apprentice.

**Content placement**

Prism cloister · Facing plinths leave a diagonal line through the glint and back to the landing.

```text
...:...
.!.:X..
..X:...
.:::::.
...:X..
..X.:!.
....:..
```

X is solid cover, : is the approach inlay, ! is an amber warning lamp, and the middle tile holds the optional prize.

- Parry glint appears on travel lines. Players pulse on the bright beat for lift fuel and a brief burst of speed while travelling.
- Focus step appears on travel lines. Players lift through the crown for lift fuel and a brief burst of speed while travelling.
- Hanging weight appears on travel lines. Players charge and release on the plate for lift fuel and a brief burst of speed while travelling.
- Prism parry is staged in a widened court. Pulse the staff when the incoming lens flare turns green. The final sequence revisits this action after the shelter.
- Counterweight pull is staged in a widened court. Hold the prism beam on the hanging weight to draw it home. The final sequence revisits this action after the shelter.
- Focus leap is staged in a widened court. Leap through the suspended focus ring. The final sequence revisits this action after the shelter.
- Prism chip sits between two entrances in the cover shown above. The orbiting amber lamp shows where contact will repel the player. Completing the indicated field skill awards a shared ward and opens the return line at the entrance.
- Prism cloister sits three tiles from the first finale anchor. It refills both seats and gives the party control over when to begin.

**Verification and playtest**

Planned intensity **2 → 5 → 7 → 4 → 2 → 9 → 2**. Built mechanical intensity **6 → 5 → 6 → 6 → 2 → 9 → 2**.

The guided path crossed **54 rooms and 1,408 tiles**. Optional room content was **33%** (25 of 75). The five sectors contained **6 independent normal-corridor cycles**. Every authored room has a reachable field reward within **10 tiles** of its centre. A reward is available in every room. The longest guided stretch outside the sensing radius of a field signal or compulsory destination was **1 tiles**.

| Player | Minutes | Marks answered | Wards found | Return rides | Longest quiet stretch | Most room entries between rewards | Climax begins |
|---|---:|---:|---:|---:|---:|---:|---:|
| Solo | 19.60 | 167 | 0 | 0 | 23.4 s | 2 | 91.0% |
| Split | 19.60 | 167 | 0 | 0 | 23.4 s | 2 | 91.0% |
| Rush | 22.69 | 9 | 0 | 0 | 78.0 s | 3 | 91.4% |
| Explore | 26.01 | 214 | 5 | 5 | 23.9 s | 2 | 93.2% |
| Cautious | 20.75 | 176 | 1 | 1 | 23.4 s | 2 | 91.5% |

**Rush.** Passing the signals left this player up to 78 seconds between successes. The field check catches a missed skill before extraction. Answering Parry glint in transit avoids a later detour.
**Explore.** Finding all five instances of Prism chip and riding home added 6.4 minutes over the guided run. The mint home marker makes the destination legible before committing.
**Cautious.** The cautious player took the ward loop in the third sector, stunned nearby threats and finished in 20.75 minutes. Prism cloister gives the player a safe place to line up the final approach.
**Revision.** The old run let players pass long stretches with a single movement input. The side room added in this revision is Prism cloister. Facing plinths leave a diagonal line through the glint and back to the landing. This door now teaches Parry glint, develops Focus step and asks for all three field responses before the final demonstration. The five new ward rooms connect from both sides. The quickest of these observed policies still had to finish the same three final encounters.

## 18 · Three Kinds of Shelter

StarCraft · 1998 · original

Connect three different forms of shelter into one evacuation route. The last shelter needs a power field, living ground and a repaired hull. Aim for recognition of three different building logics in one place.

Work the relay or follow the mat. Repair the shelter pocket for a field wafer or push on. Cross the completed mat or return through the relay court.

**Beats and graph**

Ivory relay (2) → Root crossing (5) → Blue foundry (7) → Shelter overlap (4) → Ivory shelter (2) → Evacuation mast (9) → A cooperative signal unlocks the second film (2).

Normal edges follow the spine, side passages and ward loop below. The first tool is the key to the chamber shutter. The mint return is a two-way shortcut earned in V. Sector exits are one-way after the required tools, transmission where present, and field practice. The final exit also requires three signature actions after shelter.

| Sector | Spine | Side passages | Ward loop | Route stations | Rooms / tiles | Optional rooms | Cycles | Dead ends and contents |
|---|---|---|---|---|---:|---:|---:|---|
| Ivory relay | `kfabgmrsno` | `kqlm hni` | `bcg` | `gm` | 9 / 278 | 6/15 | 2 | h fuel signal, i fuel signal, o extraction |
| Root crossing | `uvqlghinsy` | `qrm gd` | `ijn` | `ln` | 11 / 246 | 4/14 | 1 | d fuel signal, m fuel signal, u arrival landmark, y extraction |
| Blue foundry | `edijotsrmh` | `inm rq` | `tys` | `im` | 11 / 227 | 3/13 | 2 | e arrival landmark, h extraction, q fuel signal |
| Shelter overlap | `agmqwsniej` | `lmn qrs` | `mhn` | `qn` | 11 / 362 | 3/13 | 3 | a arrival landmark, j extraction, l fuel signal |
| Evacuation mast | `pklgcdjosx` | `lmno rsm` | `lqm` | `lo` | 12 / 348 | 4/14 | 3 | p arrival landmark, r fuel signal, x extraction |

- **Ivory relay** · Start k. Landmark courts kfagmo. Key Relay radius at f → a opens the chamber shutter. Fight or aim rooms m Fabricator. Puzzle or traversal rooms g Pylon field. Hinted reward c. Shortcut c ↔ k, opened at c. Lock and exit o.
- **Root crossing** · Start u. Landmark courts uvglsy. Key Living mat at v → q opens the chamber shutter. Fight or aim rooms use the active tool sentry. Puzzle or traversal rooms l Living mat, n Pylon field. Hinted reward j. Shortcut j ↔ u, opened at j. Lock and exit y.
- **Blue foundry** · Start e. Landmark courts edjorm. Key Repair fabricator at d → i opens the chamber shutter. Fight or aim rooms i Fabricator. Puzzle or traversal rooms m Living mat. Hinted reward y. Shortcut y ↔ e, opened at y. Lock and exit h.
- **Shelter overlap** · Start a. Landmark courts agmwne. Key Repair fabricator at g → m opens the chamber shutter. Fight or aim rooms m Fabricator. Puzzle or traversal rooms q Living mat, n Pylon field. Hinted reward h. Shortcut h ↔ a, opened at h. Lock and exit j.
- **Evacuation mast** · Start p. Landmark courts pkcgox. Key Relay radius at k → l opens the chamber shutter. Fight or aim rooms o Fabricator. Puzzle or traversal rooms l Pylon field, s Living mat. Hinted reward q. Shortcut q ↔ p, opened at q. Lock and exit x. Rest at Ivory shelter.

**Layout and critical walk**

```text
    Sector 1          Sector 2          Sector 3          Sector 4          Sector 5
a* b* cV                   d.                d* eS    aS          e*          c* d*
f* g* h. i.          g* h* i* jV          hE i* j*       g* hV i* jE       g*       j*
kS l. m* n* oE       l* m. n*             m* n. o*       l. m* n*       k* l* m. n. o*
   q. r. s*          q* r. s*          q. r* s* t*       q* r. s*       pS qV r. s*
                  uS v*       yE                yV          w*                   xE
```

- **Ivory relay** · f Relay radius → a Relay radius finish → g Pylon field → m Fabricator → b Repair fabricator → g Repair fabricator finish → s Recover dispatch → n Answer dispatch → o Exit. The ivory pylon stands in a cloverleaf of relay alcoves. Commission the shelter inside its field.
- **Root crossing** · v Living mat → q Living mat finish → l Living mat → n Pylon field → l Repair fabricator → g Repair fabricator finish → h Relay radius → i Relay radius finish → n Recover dispatch → s Answer dispatch → y Exit. An organic mat snakes across the cut. Grow its leading edge toward the second settlement.
- **Blue foundry** · d Repair fabricator → i Repair fabricator finish → i Fabricator → m Living mat → j Relay radius → o Relay radius finish → t Living mat → s Living mat finish → r Recover dispatch → m Answer dispatch → h Exit. Industrial bays surround a square maintenance apron. Bring construction supplies through the foundry.
- **Shelter overlap** · g Repair fabricator → m Repair fabricator finish → q Living mat → n Pylon field → m Fabricator → q Living mat → w Living mat finish → s Relay radius → n Relay radius finish → i Recover dispatch → e Answer dispatch → j Exit. Three shelter lobes touch at one contested junction. Connect the live mat to the powered relay.
- **Evacuation mast** · k Relay radius → l Relay radius finish → g Living mat → c Living mat finish → d Repair fabricator → j Repair fabricator finish → o Recover dispatch → s Answer dispatch → (25,45) Ivory shelter → l Pylon field → o Fabricator → s Living mat → x Exit. The signal mast overlooks the evacuation crescent. Finish the shared repair at the point where the shelters meet.

**Content placement**

Three shelter annexes · Three isolated footings leave an open overlap at the centre for the field wafer.

```text
..:....
.XX..!.
..:..X.
..::::.
.X..:..
.!..:X.
....:..
```

X is solid cover, : is the approach inlay, ! is an amber warning lamp, and the middle tile holds the optional prize.

- Pylon pulse appears on travel lines. Players pulse on the bright beat for lift fuel and a brief burst of speed while travelling.
- Growth node appears on travel lines. Players press Use for lift fuel and a brief burst of speed while travelling.
- Repair contact appears on travel lines. Players charge and release on the plate for lift fuel and a brief burst of speed while travelling.
- Pylon field is staged in a widened court. Tune the ivory pylon and confirm its radius. The final sequence revisits this action after the shelter.
- Living mat is staged in a widened court. Plant the living node with Use to connect its shelter. The final sequence revisits this action after the shelter.
- Fabricator is staged in a widened court. Hold the repair beam on the shelter until its hull is sealed. The final sequence revisits this action after the shelter.
- Field wafer sits between two entrances in the cover shown above. The orbiting amber lamp shows where contact will repel the player. Completing the indicated field skill awards a shared ward and opens the return line at the entrance.
- Ivory shelter sits three tiles from the first finale anchor. It refills both seats and gives the party control over when to begin.

**Verification and playtest**

Planned intensity **2 → 5 → 7 → 4 → 2 → 9 → 2**. Built mechanical intensity **5 → 5 → 6 → 6 → 2 → 8 → 2**.

The guided path crossed **54 rooms and 1,461 tiles**. Optional room content was **29%** (20 of 69). The five sectors contained **11 independent normal-corridor cycles**. Every authored room has a reachable field reward within **8 tiles** of its centre. A reward is available in every room. The longest guided stretch outside the sensing radius of a field signal or compulsory destination was **2 tiles**.

| Player | Minutes | Marks answered | Wards found | Return rides | Longest quiet stretch | Most room entries between rewards | Climax begins |
|---|---:|---:|---:|---:|---:|---:|---:|
| Solo | 19.51 | 195 | 0 | 0 | 20.1 s | 2 | 92.7% |
| Split | 19.51 | 195 | 0 | 0 | 20.1 s | 2 | 92.7% |
| Rush | 22.17 | 8 | 0 | 0 | 61.0 s | 3 | 93.0% |
| Explore | 25.01 | 235 | 5 | 5 | 20.1 s | 2 | 94.3% |
| Cautious | 20.84 | 210 | 1 | 1 | 20.1 s | 2 | 93.2% |

**Rush.** Passing the signals left this player up to 61 seconds between successes. The field check catches a missed skill before extraction. Answering Pylon pulse in transit avoids a later detour.
**Explore.** Finding all five instances of Field wafer and riding home added 5.5 minutes over the guided run. The mint home marker makes the destination legible before committing.
**Cautious.** The cautious player took the ward loop in the third sector, stunned nearby threats and finished in 20.84 minutes. Ivory shelter gives the player a safe place to line up the final approach.
**Revision.** The old run let players pass long stretches with a single movement input. The side room added in this revision is Three shelter annexes. Three isolated footings leave an open overlap at the centre for the field wafer. This door now teaches Pylon pulse, develops Growth node and asks for all three field responses before the final demonstration. The five new ward rooms connect from both sides. The quickest of these observed policies still had to finish the same three final encounters.

## 19 · Letters in the Falls

Unreal · 1998 · original campaign

Translate a valley's signs and learn the two-shot light orb. A pictograph becomes a timing instruction for a shot in the sluice. Aim for wonder followed by comprehension.

Inspect the keeper stone or go to the water. Wait for the echo pearl's answering light or stay with the keeper. Take the translated crossing or retrace the aqueduct.

**Beats and graph**

Broken lift (2) → Pictograph wall (5) → Jade sluice (7) → Keeper crossing (4) → Dry basin (2) → Falls overlook (9) → The keeper’s letter asks the hall for a route in, the first invitation written from outside (2).

Normal edges follow the spine, side passages and ward loop below. The first tool is the key to the chamber shutter. The mint return is a two-way shortcut earned in V. Sector exits are one-way after the required tools, transmission where present, and field practice. The final exit also requires three signature actions after shelter.

| Sector | Spine | Side passages | Ward loop | Route stations | Rooms / tiles | Optional rooms | Cycles | Dead ends and contents |
|---|---|---|---|---|---:|---:|---:|---|
| Broken lift | `afkpqrmhie` | `lmn ghc` | `gbh` | `km` | 10 / 269 | 8/15 | 1 | a arrival landmark, c fuel signal, e extraction, l fuel signal, n fuel signal |
| Pictograph wall | `uvwrlgbdin` | `lmn rh` | `gfb` | `ln` | 11 / 290 | 5/13 | 2 | h fuel signal, u arrival landmark |
| Jade sluice | `ejonsxwrml` | `onm hmr` | `otn` | `nr` | 9 / 224 | 3/12 | 2 | e arrival landmark, h fuel signal, l extraction |
| Keeper crossing | `kfbchmrsty` | `lmno rqw` | `cdh` | `mr` | 10 / 275 | 8/16 | 1 | k arrival landmark, l fuel signal, o fuel signal, w fuel signal, y extraction |
| Falls overlook | `vpkghinote` | `lmnr gcb` | `pqk` | `go` | 10 / 394 | 7/16 | 1 | b fuel signal, e extraction, l fuel signal, r fuel signal, v arrival landmark |

- **Broken lift** · Start a. Landmark courts afpqre. Key Light orb at f opens the chamber shutter. Fight or aim rooms k Wall translator, m Sluice orb. Puzzle or traversal rooms Stone translator. Hinted reward b. Shortcut b ↔ a, opened at b. Lock and exit e.
- **Pictograph wall** · Start u. Landmark courts uvwlbn. Key Stone translator at v → w opens the chamber shutter. Fight or aim rooms n Wall translator. Puzzle or traversal rooms l Keeper's path. Hinted reward f. Shortcut f ↔ u, opened at f. Lock and exit n.
- **Jade sluice** · Start e. Landmark courts ejosxl. Key Light orb at j opens the chamber shutter. Fight or aim rooms n Sluice orb. Puzzle or traversal rooms r Keeper's path. Hinted reward t. Shortcut t ↔ e, opened at t. Lock and exit l.
- **Keeper crossing** · Start k. Landmark courts kfchmy. Key Valley keeper at f → b opens the chamber shutter. Fight or aim rooms m Wall translator, r Sluice orb. Puzzle or traversal rooms Light orb. Hinted reward d. Shortcut d ↔ k, opened at d. Lock and exit y.
- **Falls overlook** · Start v. Landmark courts vpghoe. Key Valley keeper at p → k opens the chamber shutter. Fight or aim rooms g Wall translator, o Sluice orb. Puzzle or traversal rooms n Keeper's path. Hinted reward q. Shortcut q ↔ v, opened at q. Lock and exit e. Rest at Dry basin.

**Layout and critical walk**

```text
    Sector 1          Sector 2          Sector 3          Sector 4          Sector 5
aS bV c.    eE       b*    d.                   eS       b* c* dV          b. c.    eE
f* g. h. i.       fV g* h. i.             h.    j*    f*    h*             g* h. i*
k* l. m* n.          l* m. nE          lE m* n* o.    kS l. m* n. o.    k* l. m. n* o*
p* q* r.                r*                r* s* tV       q. r* s. t.    p* qV r.    t*
                  uS v* w*                w* x*             w.    yE       vS
```

- **Broken lift** · f Light orb → k Wall translator → m Sluice orb → p Stone translator → q Stone translator finish → e Exit. A bent lift shaft hides the valley until the last turn. Read the first stone at the overlook.
- **Pictograph wall** · v Stone translator → w Stone translator finish → l Keeper's path → n Wall translator → r Light orb → g Valley keeper → b Valley keeper finish → n Exit. The glyph wall runs along one bank of the channel. Its answer can be seen across the water.
- **Jade sluice** · j Light orb → n Sluice orb → r Keeper's path → n Stone translator → s Stone translator finish → x Valley keeper → w Valley keeper finish → r Recover dispatch → m Answer dispatch → l Exit. An elbow in the aqueduct hides the lower sluice. Prime the seal from the dry inspection platform.
- **Keeper crossing** · f Valley keeper → b Valley keeper finish → m Wall translator → r Sluice orb → c Stone translator → h Stone translator finish → m Light orb → y Exit. Follow the keeper down through the broken aqueduct. Its stepping stones lead around a sheer drop.
- **Falls overlook** · p Valley keeper → k Valley keeper finish → g Light orb → i Stone translator → n Stone translator finish → o Recover dispatch → t Answer dispatch → (25,28) Dry basin → g Wall translator → o Sluice orb → n Keeper's path → e Exit. The climb curls behind the falls. Carry the keeper's letter to daylight on the far bank.

**Content placement**

Echo grotto · A hooked viewing shelf faces an open pool with space to track the answering light.

```text
....:..
.XX.:!.
.X..:..
..:::..
..:....
.!:..X.
..:::..
```

X is solid cover, : is the approach inlay, ! is an amber warning lamp, and the middle tile holds the optional prize.

- Blooming echo appears on travel lines. Players pulse on the bright beat for lift fuel and a brief burst of speed while travelling.
- Sluice lens appears on travel lines. Players aim and pulse for lift fuel and a brief burst of speed while travelling.
- Keeper stone appears on travel lines. Players press Use for lift fuel and a brief burst of speed while travelling.
- Wall translator is staged in a widened court. Tune the lens until the glyph matches and pulse. The final sequence revisits this action after the shelter.
- Sluice orb is staged in a widened court. Pulse to launch the light orb, then shoot it when it blooms. The final sequence revisits this action after the shelter.
- Keeper's path is staged in a widened court. Use the translated waystone to mark the keeper's path. The final sequence revisits this action after the shelter.
- Echo pearl sits between two entrances in the cover shown above. The orbiting amber lamp shows where contact will repel the player. Completing the indicated field skill awards a shared ward and opens the return line at the entrance.
- Dry basin sits three tiles from the first finale anchor. It refills both seats and gives the party control over when to begin.

**Verification and playtest**

Planned intensity **2 → 5 → 7 → 4 → 2 → 9 → 2**. Built mechanical intensity **6 → 5 → 7 → 6 → 2 → 9 → 2**.

The guided path crossed **50 rooms and 1,452 tiles**. Optional room content was **43%** (31 of 72). The five sectors contained **7 independent normal-corridor cycles**. Every authored room has a reachable field reward within **8 tiles** of its centre. A reward is available in every room. The longest guided stretch outside the sensing radius of a field signal or compulsory destination was **1 tiles**.

| Player | Minutes | Marks answered | Wards found | Return rides | Longest quiet stretch | Most room entries between rewards | Climax begins |
|---|---:|---:|---:|---:|---:|---:|---:|
| Solo | 19.73 | 204 | 0 | 0 | 16.2 s | 2 | 90.4% |
| Split | 19.72 | 204 | 0 | 0 | 16.2 s | 2 | 90.4% |
| Rush | 22.90 | 8 | 0 | 0 | 78.1 s | 4 | 90.9% |
| Explore | 25.67 | 257 | 5 | 5 | 16.1 s | 2 | 92.6% |
| Cautious | 20.39 | 212 | 1 | 1 | 15.6 s | 2 | 90.7% |

**Rush.** Passing the signals left this player up to 78 seconds between successes. The field check catches a missed skill before extraction. Answering Blooming echo in transit avoids a later detour.
**Explore.** Finding all five instances of Echo pearl and riding home added 5.9 minutes over the guided run. The mint home marker makes the destination legible before committing.
**Cautious.** The cautious player took the ward loop in the third sector, stunned nearby threats and finished in 20.39 minutes. Dry basin gives the player a safe place to line up the final approach.
**Revision.** The old run let players pass long stretches with a single movement input. The side room added in this revision is Echo grotto. A hooked viewing shelf faces an open pool with space to track the answering light. This door now teaches Blooming echo, develops Sluice lens and asks for all three field responses before the final demonstration. The five new ward rooms connect from both sides. The quickest of these observed policies still had to finish the same three final encounters.

## 20 · Dead Air on Deck Nine

System Shock · 1994 · Looking Glass original

Reconstruct an address by disabling surveillance and reconnecting a circuit. A network jack links rooms that are far apart on the physical deck. Aim for a moment of clarity inside an untrustworthy station.

Blind the eye or skirt its room. Trace the archive fork for a log spool or follow the dispatch address. Jack back across the deck or stay in physical space.

**Beats and graph**

Maintenance lock (2) → Silent ward (5) → Lens deck (7) → Wireframe circuit (4) → Silent ward (2) → Dispatcher bay (9) → A spool of message tape records the Steward practising the words COME IN (2).

Normal edges follow the spine, side passages and ward loop below. The first tool is the key to the chamber shutter. The mint return is a two-way shortcut earned in V. Sector exits are one-way after the required tools, transmission where present, and field practice. The final exit also requires three signature actions after shelter.

| Sector | Spine | Side passages | Ward loop | Route stations | Rooms / tiles | Optional rooms | Cycles | Dead ends and contents |
|---|---|---|---|---|---:|---:|---:|---|
| Maintenance lock | `upklmhgbcd` | `qrm hijn` | `gfb` | `lh` | 10 / 228 | 7/16 | 1 | d extraction, n fuel signal, q fuel signal, u arrival landmark |
| Silent ward | `ejotsrmlgb` | `inml rqw` | `ihn` | `om` | 10 / 262 | 5/15 | 1 | b extraction, e arrival landmark, w fuel signal |
| Lens deck | `abchinotxw` | `hgl msr hm` | `afb` | `hn` | 10 / 243 | 7/16 | 1 | l fuel signal, r fuel signal, w extraction |
| Wireframe circuit | `mgaceioswq` | `mhc rsm` | `eji` | `mw` | 10 / 425 | 4/13 | 3 | q extraction, r fuel signal |
| Dispatcher bay | `ytonidchlk` | `nmh dcg` | `nsm` | `nh` | 11 / 337 | 3/13 | 2 | g fuel signal, k extraction, y arrival landmark |

- **Maintenance lock** · Start u. Landmark courts upklhd. Key Log fragments at p → k opens the chamber shutter. Fight or aim rooms h Surveillance eyes. Puzzle or traversal rooms l Log address. Hinted reward f. Shortcut f ↔ u, opened at f. Lock and exit d.
- **Silent ward** · Start e. Landmark courts ejoslb. Key Surveillance lens at j → o opens the chamber shutter. Fight or aim rooms o Surveillance eyes, m Deck security. Puzzle or traversal rooms Circuit nodes. Hinted reward h. Shortcut h ↔ e, opened at h. Lock and exit b.
- **Lens deck** · Start a. Landmark courts abchio. Key Surveillance lens at b → c opens the chamber shutter. Fight or aim rooms h Deck security, n Surveillance eyes. Puzzle or traversal rooms Log fragments. Hinted reward f. Shortcut f ↔ a, opened at f. Lock and exit w.
- **Wireframe circuit** · Start m. Landmark courts mgceiw. Key Circuit nodes at g opens the chamber shutter. Fight or aim rooms w Deck security. Puzzle or traversal rooms m Log address. Hinted reward j. Shortcut j ↔ m, opened at j. Lock and exit q.
- **Dispatcher bay** · Start y. Landmark courts ytonlk. Key Circuit nodes at t opens the chamber shutter. Fight or aim rooms n Surveillance eyes, m Deck security. Puzzle or traversal rooms h Log address. Hinted reward s. Shortcut s ↔ y, opened at s. Lock and exit k. Rest at Silent ward.

**Layout and critical walk**

```text
    Sector 1          Sector 2          Sector 3          Sector 4          Sector 5
   b* c* dE          bE       eS    aS b* c*          a.    c*    e*          c* d*
fV g* h* i. j.       g* hV i. j*    fV g. h* i.          g* h. i* jV       g. h* i*
k* l* m. n.          l* m* n. o*       l. m. n* o*          mS    o*    kE l* m* n* o.
p* q. r.             q. r* s* t*          r. s. t*       qE r. s*                sV t*
uS                      w.                wE x*             w*                      yS
```

- **Maintenance lock** · p Log fragments → k Log fragments finish → l Log address → h Surveillance eyes → l Circuit nodes → h Surveillance lens → g Surveillance lens finish → b Recover dispatch → c Answer dispatch → d Exit. Trace the missing dispatcher from the maintenance airlock along its crooked service conduit.
- **Silent ward** · j Surveillance lens → o Surveillance lens finish → o Surveillance eyes → m Deck security → t Log fragments → s Log fragments finish → r Circuit nodes → l Recover dispatch → g Answer dispatch → b Exit. Two ward corridors divide into small camera bays. Blind the eye watching the records terminal.
- **Lens deck** · b Surveillance lens → c Surveillance lens finish → h Deck security → n Surveillance eyes → h Circuit nodes → n Log fragments → o Log fragments finish → t Recover dispatch → x Answer dispatch → w Exit. A camera overlooks the bend on the lens deck. Cut its feed to expose the circuit controls.
- **Wireframe circuit** · g Circuit nodes → m Log address → w Deck security → c Log fragments → e Log fragments finish → i Surveillance lens → o Surveillance lens finish → s Recover dispatch → w Answer dispatch → q Exit. Inside the network, the deck becomes a bright circuit. Cross the spokes between memory addresses.
- **Dispatcher bay** · t Circuit nodes → n Surveillance lens → i Surveillance lens finish → d Log fragments → c Log fragments finish → h Recover dispatch → l Answer dispatch → (59,45) Silent ward → n Surveillance eyes → h Log address → m Deck security → k Exit. Follow the recovered signal to the dispatcher bay. The last camera looks into the empty chair.

**Content placement**

Archive fork · Three short equipment racks divide the physical ward into bays around one jack.

```text
.:...:.
.:XX.!.
.:.....
.:::::.
.....:.
.!XX.:.
.:...:.
```

X is solid cover, : is the approach inlay, ! is an amber warning lamp, and the middle tile holds the optional prize.

- Tracking eye appears on travel lines. Players aim and pulse for lift fuel and a brief burst of speed while travelling.
- Log jack appears on travel lines. Players press Use for lift fuel and a brief burst of speed while travelling.
- Circuit pulse appears on travel lines. Players pulse on the bright beat for lift fuel and a brief burst of speed while travelling.
- Surveillance eyes is staged in a widened court. Blind the three tracking eyes with aimed pulses. The final sequence revisits this action after the shelter.
- Deck security is staged in a widened court. Tune the security loop and pulse to disconnect it. The final sequence revisits this action after the shelter.
- Log address is staged in a widened court. Use the jack to recover the dispatcher's address. The final sequence revisits this action after the shelter.
- Archive spool sits between two entrances in the cover shown above. The orbiting amber lamp shows where contact will repel the player. Completing the indicated field skill awards a shared ward and opens the return line at the entrance.
- Silent ward sits three tiles from the first finale anchor. It refills both seats and gives the party control over when to begin.

**Verification and playtest**

Planned intensity **2 → 5 → 7 → 4 → 2 → 9 → 2**. Built mechanical intensity **5 → 5 → 6 → 5 → 2 → 8 → 2**.

The guided path crossed **51 rooms and 1,495 tiles**. Optional room content was **36%** (26 of 73). The five sectors contained **8 independent normal-corridor cycles**. Every authored room has a reachable field reward within **8 tiles** of its centre. A reward is available in every room. The longest guided stretch outside the sensing radius of a field signal or compulsory destination was **2 tiles**.

| Player | Minutes | Marks answered | Wards found | Return rides | Longest quiet stretch | Most room entries between rewards | Climax begins |
|---|---:|---:|---:|---:|---:|---:|---:|
| Solo | 20.05 | 185 | 0 | 0 | 18.1 s | 2 | 92.3% |
| Split | 20.04 | 185 | 0 | 0 | 18.1 s | 2 | 92.3% |
| Rush | 22.73 | 8 | 0 | 0 | 81.9 s | 4 | 92.7% |
| Explore | 26.06 | 239 | 5 | 5 | 20.8 s | 2 | 94.1% |
| Cautious | 20.26 | 189 | 1 | 1 | 18.1 s | 2 | 92.4% |

**Rush.** Passing the signals left this player up to 82 seconds between successes. The field check catches a missed skill before extraction. Answering Tracking eye in transit avoids a later detour.
**Explore.** Finding all five instances of Archive spool and riding home added 6.0 minutes over the guided run. The mint home marker makes the destination legible before committing.
**Cautious.** The cautious player took the ward loop in the third sector, stunned nearby threats and finished in 20.26 minutes. Silent ward gives the player a safe place to line up the final approach.
**Revision.** The old run let players pass long stretches with a single movement input. The side room added in this revision is Archive fork. Three short equipment racks divide the physical ward into bays around one jack. This door now teaches Tracking eye, develops Log jack and asks for all three field responses before the final demonstration. The five new ward rooms connect from both sides. The quickest of these observed policies still had to finish the same three final encounters.

## 21 · A Village Before Snow

Age of Empires · 1997 · first game

Bring labour, food and a working crossing together for a winter feast. The ford becomes more valuable once you have something to carry. Aim for satisfaction at improving a small settlement.

Ring a work bell or carry food straight through. Recover the mill tally or return to the foundation. Cross the finished ford or take the bank with the stores.

**Beats and graph**

Hearth square (2) → Timber yard (5) → Stone ford (7) → Flood crossing (4) → Hearth bench (2) → Feast house (9) → A loaf carved from alder wood marks a place for anyone who arrives hungry (2).

Normal edges follow the spine, side passages and ward loop below. The first tool is the key to the chamber shutter. The mint return is a two-way shortcut earned in V. Sector exits are one-way after the required tools, transmission where present, and field practice. The final exit also requires three signature actions after shelter.

| Sector | Spine | Side passages | Ward loop | Route stations | Rooms / tiles | Optional rooms | Cycles | Dead ends and contents |
|---|---|---|---|---|---:|---:|---:|---|
| Hearth square | `mlkfbchrwv` | `mno qrs` | `bgc` | `mh` | 8 / 238 | 7/15 | 1 | o fuel signal, q fuel signal, s fuel signal, v extraction |
| Timber yard | `afkpuvqrmh` | `klm rsto` | `mnh` | `km` | 11 / 188 | 6/15 | 2 | a arrival landmark, o fuel signal |
| Stone ford | `edinsxwvql` | `onm rsq la` | `otn` | `iv` | 12 / 240 | 7/15 | 2 | a fuel signal, e arrival landmark, m fuel signal, r fuel signal |
| Flood crossing | `upqlghioty` | `lmn rvw uv` | `hci` | `lo` | 12 / 232 | 7/16 | 1 | n fuel signal, r fuel signal, w fuel signal, y extraction |
| Feast house | `wrqlfacejo` | `mnr gch` | `jio` | `rc` | 12 / 392 | 4/15 | 1 | g fuel signal, h fuel signal, m tool / field reward, w arrival landmark |

- **Hearth square** · Start m. Landmark courts mlkbhv. Key Work orders at l → k opens the chamber shutter. Fight or aim rooms h Granary balance. Puzzle or traversal rooms m Villager order. Hinted reward g. Shortcut g ↔ m, opened at g. Lock and exit v.
- **Timber yard** · Start a. Landmark courts afkuvh. Key Work orders at f → k opens the chamber shutter. Fight or aim rooms m Raised foundation. Puzzle or traversal rooms k Villager order. Hinted reward n. Shortcut n ↔ a, opened at n. Lock and exit h.
- **Stone ford** · Start e. Landmark courts edianv. Key Food stores at d → i opens the chamber shutter. Fight or aim rooms i Granary balance. Puzzle or traversal rooms v Villager order. Hinted reward t. Shortcut t ↔ e, opened at t. Lock and exit l.
- **Flood crossing** · Start u. Landmark courts upqgty. Key Feast foundation at p → q opens the chamber shutter. Fight or aim rooms l Raised foundation. Puzzle or traversal rooms o Villager order. Hinted reward c. Shortcut c ↔ u, opened at c. Lock and exit y.
- **Feast house** · Start w. Landmark courts wrqaco. Key Food stores at r → q opens the chamber shutter. Fight or aim rooms c Granary balance, m Raised foundation. Puzzle or traversal rooms r Villager order. Hinted reward i. Shortcut i ↔ w, opened at i. Lock and exit o. Rest at Hearth bench.

**Layout and critical walk**

```text
    Sector 1          Sector 2          Sector 3          Sector 4          Sector 5
   b* c*          aS                a.       d* eS          cV          a*    c*    e*
f* gV h*          f*    hE                   i*          g* h* i*       f* g. h. iV j*
k* l* mS n. o.    k* l. m* nV o.       lE m. n* o.       l* m. n. o*       l* m* n. oE
   q. r. s.       p* q* r. s. t.       q* r. s* tV    p* q* r.    t.       q* r*
   vE w.          u* v*                v* w. x.       uS v. w.    yE          wS
```

- **Hearth square** · l Work orders → k Work orders finish → m Villager order → h Granary balance → f Food stores → b Food stores finish → c Feast foundation → h Feast foundation finish → v Exit. Three plots face a common hearth. Send the first workers out along the village lanes.
- **Timber yard** · f Work orders → k Work orders finish → k Villager order → m Raised foundation → p Feast foundation → u Feast foundation finish → v Food stores → q Food stores finish → h Exit. The timber settlement hugs a single forest edge. Haul the stock back by the cart track.
- **Stone ford** · d Food stores → i Food stores finish → i Granary balance → v Villager order → n Work orders → s Work orders finish → v Recover dispatch → q Answer dispatch → l Exit. The quarry and granary lie on opposing banks. Bring food to the work crews at the ford.
- **Flood crossing** · p Feast foundation → q Feast foundation finish → l Raised foundation → o Villager order → l Work orders → g Work orders finish → h Food stores → i Food stores finish → y Exit. Build out from the floodplain. A timber crossing brings the two halves of the settlement together.
- **Feast house** · r Food stores → q Food stores finish → l Feast foundation → f Feast foundation finish → a Work orders → c Work orders finish → e Recover dispatch → j Answer dispatch → (36,53) Hearth bench → r Villager order → c Granary balance → m Raised foundation → o Exit. The feast house rises above a market horseshoe. Carry the last stores past the occupied foundations.

**Content placement**

Counting mill · The grain track circles a milling stone and rejoins the cart road by the tally.

```text
.:::...
.:.X.!.
.:..X..
.:::::.
..X..:.
.!...:.
...:::.
```

X is solid cover, : is the approach inlay, ! is an amber warning lamp, and the middle tile holds the optional prize.

- Work bell appears on travel lines. Players press Use for lift fuel and a brief burst of speed while travelling.
- Cart track appears on travel lines. Players cross at ground height for lift fuel and a brief burst of speed while travelling.
- Roof joint appears on travel lines. Players charge and release on the plate for lift fuel and a brief burst of speed while travelling.
- Villager order is staged in a widened court. Use the job bell to assign the waiting workers. The final sequence revisits this action after the shelter.
- Granary balance is staged in a widened court. Set the food allocation and pulse to advance the village. The final sequence revisits this action after the shelter.
- Raised foundation is staged in a widened court. Keep the builder's beam on the frame to finish the roof. The final sequence revisits this action after the shelter.
- Mill tally sits between two entrances in the cover shown above. The orbiting amber lamp shows where contact will repel the player. Completing the indicated field skill awards a shared ward and opens the return line at the entrance.
- Hearth bench sits three tiles from the first finale anchor. It refills both seats and gives the party control over when to begin.

**Verification and playtest**

Planned intensity **2 → 5 → 7 → 4 → 2 → 9 → 2**. Built mechanical intensity **5 → 5 → 6 → 5 → 2 → 8 → 2**.

The guided path crossed **55 rooms and 1,290 tiles**. Optional room content was **41%** (31 of 76). The five sectors contained **7 independent normal-corridor cycles**. Every authored room has a reachable field reward within **9 tiles** of its centre. A reward is available in every room. The longest guided stretch outside the sensing radius of a field signal or compulsory destination was **3 tiles**.

| Player | Minutes | Marks answered | Wards found | Return rides | Longest quiet stretch | Most room entries between rewards | Climax begins |
|---|---:|---:|---:|---:|---:|---:|---:|
| Solo | 17.74 | 154 | 0 | 0 | 21.3 s | 2 | 88.3% |
| Split | 17.74 | 154 | 0 | 0 | 21.3 s | 2 | 88.3% |
| Rush | 20.13 | 61 | 0 | 0 | 37.8 s | 3 | 89.1% |
| Explore | 23.62 | 188 | 5 | 5 | 46.2 s | 2 | 91.2% |
| Cautious | 18.69 | 158 | 1 | 1 | 43.6 s | 2 | 88.9% |

**Rush.** Passing the signals left this player up to 38 seconds between successes. The field check catches a missed skill before extraction. Answering Work bell in transit avoids a later detour.
**Explore.** Finding all five instances of Mill tally and riding home added 5.9 minutes over the guided run. The mint home marker makes the destination legible before committing.
**Cautious.** The cautious player took the ward loop in the third sector, stunned nearby threats and finished in 18.69 minutes. Hearth bench gives the player a safe place to line up the final approach.
**Revision.** The old run let players pass long stretches with a single movement input. The side room added in this revision is Counting mill. The grain track circles a milling stone and rejoins the cart road by the tally. This door now teaches Work bell, develops Cart track and asks for all three field responses before the final demonstration. The five new ward rooms connect from both sides. The quickest of these observed policies still had to finish the same three final encounters.

## 22 · Last Train to the Midway

Blood · 1997 · original

Recognise the real carnival signal by priming a flare and reading its bloom. The familiar light becomes a decoy in the mirror tent. Aim for suspicion giving way to a well-timed shot.

Mark the flare on its beat or skim past. Search the sideshow for a mirror ticket or stay near the carriage. Return through the service line or work the midway again.

**Beats and graph**

Rear carriage (2) → Sideshow platform (5) → Mirror tent (7) → False signal (4) → Empty carriage (2) → Midnight train (9) → A carnival handbill advertises the reunion with its correct, previously unprinted return address (2).

Normal edges follow the spine, side passages and ward loop below. The first tool is the key to the chamber shutter. The mint return is a two-way shortcut earned in V. Sector exits are one-way after the required tools, transmission where present, and field practice. The final exit also requires three signature actions after shelter.

| Sector | Spine | Side passages | Ward loop | Route stations | Rooms / tiles | Optional rooms | Cycles | Dead ends and contents |
|---|---|---|---|---|---:|---:|---:|---|
| Rear carriage | `upkfabglqv` | `kl qv` | `bcg` | `kq` | 10 / 185 | 2/11 | 2 | u arrival landmark, v extraction |
| Sideshow platform | `bcdinsxwrq` | `hmn rsv` | `ijn` | `ir` | 9 / 211 | 7/14 | 2 | b arrival landmark, h fuel signal, q extraction, v fuel signal |
| Mirror tent | `kgcionmlqw` | `mhs qrs` | `gbc` | `gm` | 10 / 335 | 5/14 | 2 | k arrival landmark, w extraction |
| False signal | `ytojedinsx` | `onm xwr` | `ihn` | `os` | 10 / 189 | 4/14 | 2 | m fuel signal, r fuel signal, y arrival landmark |
| Midnight train | `afkpuvwxsn` | `qrl gch kl fg` | `xys` | `kn` | 12 / 415 | 7/17 | 1 | a arrival landmark, h tool / field reward, n extraction, q fuel signal |

- **Rear carriage** · Start u. Landmark courts upkagv. Key Lantern flare at p → k opens the chamber shutter. Fight or aim rooms k Delayed flare, q Twin-caster gallery. Puzzle or traversal rooms Twin-caster token. Hinted reward c. Shortcut c ↔ u, opened at c. Lock and exit v.
- **Sideshow platform** · Start b. Landmark courts bcinwq. Key Burst caster at c opens the chamber shutter. Fight or aim rooms i Twin-caster gallery, r Mirror signal. Puzzle or traversal rooms Lantern flare. Hinted reward j. Shortcut j ↔ b, opened at j. Lock and exit q.
- **Mirror tent** · Start k. Landmark courts kgcomw. Key Twin-caster token at g → c opens the chamber shutter. Fight or aim rooms g Mirror signal, m Delayed flare. Puzzle or traversal rooms Lantern flare. Hinted reward b. Shortcut b ↔ k, opened at b. Lock and exit w.
- **False signal** · Start y. Landmark courts ytoedx. Key Burst caster at t opens the chamber shutter. Fight or aim rooms o Delayed flare, s Mirror signal. Puzzle or traversal rooms Twin-caster token. Hinted reward h. Shortcut h ↔ y, opened at h. Lock and exit x.
- **Midnight train** · Start a. Landmark courts afkuwn. Key Twin-caster token at f → k opens the chamber shutter. Fight or aim rooms k Twin-caster gallery, h Delayed flare, n Mirror signal. Puzzle or traversal rooms Burst caster. Hinted reward y. Shortcut y ↔ a, opened at y. Lock and exit n. Rest at Empty carriage.

**Layout and critical walk**

```text
    Sector 1          Sector 2          Sector 3          Sector 4          Sector 5
a. b* cV             bS c* d.          bV c*                   d* e*    aS    c.
f* g*                   h. i* jV       g* h. i*             hV i* j*    f* g. h*
k* l*                   m. n*       kS l* m* n* o.          m. n* o*    k* l.    nE
p* q*                qE r* s.          q* r. s.             r. s* t*    p* q. r. s*
uS vE                v. w* x.             wE                w. xE yS    u* v* w. x* yV
```

- **Rear carriage** · p Lantern flare → k Lantern flare finish → k Delayed flare → q Twin-caster gallery → f Burst caster → b Twin-caster token → g Twin-caster token finish → l Recover dispatch → q Answer dispatch → v Exit. Work up the carriage aisle and back along the narrow platform. Wax conductors wait in the vestibules.
- **Sideshow platform** · c Burst caster → i Twin-caster gallery → r Mirror signal → i Lantern flare → n Lantern flare finish → w Recover dispatch → r Answer dispatch → q Exit. The sideshow spills sideways from a long platform. Light the signal at the end of the blind siding.
- **Mirror tent** · g Twin-caster token → c Twin-caster token finish → g Mirror signal → m Delayed flare → i Burst caster → n Lantern flare → m Lantern flare finish → l Recover dispatch → q Answer dispatch → w Exit. The mirror tent doubles back around its painted centre. Take the twin token past the false silhouettes.
- **False signal** · t Burst caster → o Delayed flare → s Mirror signal → j Lantern flare → e Lantern flare finish → d Twin-caster token → i Twin-caster token finish → n Recover dispatch → s Answer dispatch → x Exit. Twin signal galleries run side by side. A flare distinguishes the wax conductor at the far head.
- **Midnight train** · f Twin-caster token → k Twin-caster token finish → p Lantern flare → u Lantern flare finish → v Burst caster → x Recover dispatch → s Answer dispatch → (8,39) Empty carriage → k Twin-caster gallery → h Delayed flare → n Mirror signal → n Exit. The return train follows an L-shaped platform. Prime the last false signal before the procession arrives.

**Content placement**

Mirror-ticket booth · Staggered flats hide the second approach without concealing the bright timing signal.

```text
..:....
.X:..!.
..:.XX.
..::::.
.XX.:..
.!..:X.
....:..
```

X is solid cover, : is the approach inlay, ! is an amber warning lamp, and the middle tile holds the optional prize.

- Mirror glint appears on travel lines. Players pulse on the bright beat for lift fuel and a brief burst of speed while travelling.
- Wax silhouette appears on travel lines. Players aim and pulse for lift fuel and a brief burst of speed while travelling.
- Flare wick appears on travel lines. Players charge and release on the plate for lift fuel and a brief burst of speed while travelling.
- Delayed flare is staged in a widened court. Tag the wax signal and pulse it again as the flare blooms. The final sequence revisits this action after the shelter.
- Twin-caster gallery is staged in a widened court. Shoot the three passing wax silhouettes. The final sequence revisits this action after the shelter.
- Mirror signal is staged in a widened court. Pulse the real signal on the bright beat. The final sequence revisits this action after the shelter.
- Mirror ticket sits between two entrances in the cover shown above. The orbiting amber lamp shows where contact will repel the player. Completing the indicated field skill awards a shared ward and opens the return line at the entrance.
- Empty carriage sits three tiles from the first finale anchor. It refills both seats and gives the party control over when to begin.

**Verification and playtest**

Planned intensity **2 → 5 → 7 → 4 → 2 → 9 → 2**. Built mechanical intensity **6 → 6 → 7 → 6 → 2 → 9 → 2**.

The guided path crossed **51 rooms and 1,335 tiles**. Optional room content was **36%** (25 of 70). The five sectors contained **9 independent normal-corridor cycles**. Every authored room has a reachable field reward within **9 tiles** of its centre. A reward is available in every room. The longest guided stretch outside the sensing radius of a field signal or compulsory destination was **1 tiles**.

| Player | Minutes | Marks answered | Wards found | Return rides | Longest quiet stretch | Most room entries between rewards | Climax begins |
|---|---:|---:|---:|---:|---:|---:|---:|
| Solo | 18.12 | 191 | 0 | 0 | 10.2 s | 2 | 83.6% |
| Split | 18.12 | 191 | 0 | 0 | 10.2 s | 2 | 83.6% |
| Rush | 23.70 | 8 | 0 | 0 | 139.4 s | 10 | 86.1% |
| Explore | 23.09 | 239 | 5 | 5 | 14.1 s | 2 | 87.2% |
| Cautious | 18.63 | 197 | 1 | 1 | 11.6 s | 2 | 84.2% |

**Rush.** Passing the signals left this player up to 139 seconds between successes. The field check catches a missed skill before extraction. Answering Mirror glint in transit avoids a later detour.
**Explore.** Finding all five instances of Mirror ticket and riding home added 5.0 minutes over the guided run. The mint home marker makes the destination legible before committing.
**Cautious.** The cautious player took the ward loop in the third sector, stunned nearby threats and finished in 18.63 minutes. Empty carriage gives the player a safe place to line up the final approach.
**Revision.** The old run let players pass long stretches with a single movement input. The side room added in this revision is Mirror-ticket booth. Staggered flats hide the second approach without concealing the bright timing signal. This door now teaches Mirror glint, develops Wax silhouette and asks for all three field responses before the final demonstration. The five new ward rooms connect from both sides. The quickest of these observed policies still had to finish the same three final encounters.

## 23 · What the Winter Leaves

Total Annihilation · 1997 · original

Reclaim metal and reconnect power before finishing a constructor order. The bridge over the wreck field is built from what you found below it. Aim for a visible connection between salvage and construction.

Stop on a reclaim contact or go to the power bus. Recover the armature from the scrap pocket or carry on building. Use the completed trestle or follow the wreck-bed route.

**Beats and graph**

Chassis field (2) → Wind farm (5) → Metal basin (7) → Relay arms (4) → Lee of the windmill (2) → Aerial summit (9) → A transmitter aerial rises from the hall roof (2).

Normal edges follow the spine, side passages and ward loop below. The first tool is the key to the chamber shutter. The mint return is a two-way shortcut earned in V. Sector exits are one-way after the required tools, transmission where present, and field practice. The final exit also requires three signature actions after shelter.

| Sector | Spine | Side passages | Ward loop | Route stations | Rooms / tiles | Optional rooms | Cycles | Dead ends and contents |
|---|---|---|---|---|---:|---:|---:|---|
| Chassis field | `aglkpvwrmi` | `qrs hmn` | `gfl` | `lm` | 10 / 372 | 5/15 | 1 | a arrival landmark, h fuel signal, i extraction, n fuel signal, q fuel signal, s fuel signal |
| Wind farm | `einswvqlfb` | `nmh qrm` | `qpl` | `nl` | 12 / 328 | 4/14 | 2 | b extraction, e arrival landmark, h fuel signal |
| Metal basin | `uvqrmhcdjo` | `lmno ghr` | `nso` | `qh` | 10 / 230 | 4/14 | 3 | g fuel signal, l fuel signal, u arrival landmark |
| Relay arms | `kfbcdiotxw` | `chg rsn in` | `hmg` | `ci` | 10 / 262 | 6/16 | 1 | k arrival landmark, r fuel signal, w extraction |
| Aerial summit | `ysnmlgbchr` | `rqv ins` | `lkg` | `nr` | 10 / 248 | 4/14 | 1 | i fuel signal, v fuel signal, y arrival landmark |

- **Chassis field** · Start a. Landmark courts aglkwi. Key Reclaim beam at g → l opens the chamber shutter. Fight or aim rooms l Wreck reclaim. Puzzle or traversal rooms m Constructor order. Hinted reward f. Shortcut f ↔ a, opened at f. Lock and exit i.
- **Wind farm** · Start e. Landmark courts einwvb. Key Power balance at i → n opens the chamber shutter. Fight or aim rooms n Streaming power, l Wreck reclaim. Puzzle or traversal rooms Constructor queue. Hinted reward p. Shortcut p ↔ e, opened at p. Lock and exit b.
- **Metal basin** · Start u. Landmark courts uvqmcj. Key Constructor queue at v → q opens the chamber shutter. Fight or aim rooms q Wreck reclaim, h Streaming power. Puzzle or traversal rooms Power balance. Hinted reward s. Shortcut s ↔ u, opened at s. Lock and exit o.
- **Relay arms** · Start k. Landmark courts kfbdiw. Key Power balance at f → b opens the chamber shutter. Fight or aim rooms i Wreck reclaim. Puzzle or traversal rooms c Constructor order. Hinted reward m. Shortcut m ↔ k, opened at m. Lock and exit w.
- **Aerial summit** · Start y. Landmark courts ysmlbr. Key Constructor queue at s → n opens the chamber shutter. Fight or aim rooms n Wreck reclaim, r Streaming power. Puzzle or traversal rooms m Constructor order. Hinted reward k. Shortcut k ↔ y, opened at k. Lock and exit r. Rest at Lee of the windmill.

**Layout and critical walk**

```text
    Sector 1          Sector 2          Sector 3          Sector 4          Sector 5
aS                   bE       eS          c* d*          b* c* d*          b* c*
fV g* h. iE       f*    h. i*          g. h*    j*    f* g. h. i*          g* h* i.
k* l* m* n.          l* m. n*          l. m* n. oE    kS    mV n. o*    kV l* m* n*
p* q. r* s.       pV q* r. s*          q* r* sV             r. s. t*       q. rE s*
   v* w*             v* w*          uS v*                   wE x*          v.       yS
```

- **Chassis field** · g Reclaim beam → l Reclaim beam finish → l Wreck reclaim → m Constructor order → k Constructor queue → p Constructor queue finish → v Power balance → w Power balance finish → r Recover dispatch → m Answer dispatch → i Exit. Three wreck clusters break up the valley floor. Reclaim intact hulls before leaving the chassis field.
- **Wind farm** · i Power balance → n Power balance finish → n Streaming power → l Wreck reclaim → s Reclaim beam → w Reclaim beam finish → v Constructor queue → q Constructor queue finish → l Recover dispatch → f Answer dispatch → b Exit. A hooked ridge carries the wind farm. Link the exposed turbines back to the constructor below.
- **Metal basin** · v Constructor queue → q Constructor queue finish → q Wreck reclaim → h Streaming power → r Reclaim beam → m Reclaim beam finish → h Power balance → c Power balance finish → d Recover dispatch → j Answer dispatch → o Exit. Two metal deposits feed the basin from different sides. Balance the bus at their shared outlet.
- **Relay arms** · f Power balance → b Power balance finish → c Constructor order → i Wreck reclaim → c Constructor queue → d Constructor queue finish → i Reclaim beam → o Reclaim beam finish → t Recover dispatch → x Answer dispatch → w Exit. The northern relay arm bends east while its companion crosses the basin. Rebuild the return feed.
- **Aerial summit** · s Constructor queue → n Constructor queue finish → m Power balance → l Power balance finish → g Reclaim beam → b Reclaim beam finish → c Recover dispatch → h Answer dispatch → (56,43) Lee of the windmill → n Wreck reclaim → r Streaming power → m Constructor order → r Exit. Climb through the debris to a rival hulk on the ridge. Salvage it into the transmitter aerial.

**Content placement**

Armature bed · A split wreck hull frames the reclaim plate and leaves two ways to reach the winding.

```text
..:::..
.!:.XX.
..:..X.
..:::..
.X..:..
.XX.:!.
..:::..
```

X is solid cover, : is the approach inlay, ! is an amber warning lamp, and the middle tile holds the optional prize.

- Reclaim contact appears on travel lines. Players charge and release on the plate for lift fuel and a brief burst of speed while travelling.
- Power winding appears on travel lines. Players pulse on the bright beat for lift fuel and a brief burst of speed while travelling.
- Constructor stud appears on travel lines. Players press Use for lift fuel and a brief burst of speed while travelling.
- Wreck reclaim is staged in a widened court. Hold the constructor beam on the hulk to reclaim its metal. The final sequence revisits this action after the shelter.
- Streaming power is staged in a widened court. Balance the power bus and pulse to feed the queue. The final sequence revisits this action after the shelter.
- Constructor order is staged in a widened court. Use the order marker to dispatch the helper. The final sequence revisits this action after the shelter.
- Spare armature sits between two entrances in the cover shown above. The orbiting amber lamp shows where contact will repel the player. Completing the indicated field skill awards a shared ward and opens the return line at the entrance.
- Lee of the windmill sits three tiles from the first finale anchor. It refills both seats and gives the party control over when to begin.

**Verification and playtest**

Planned intensity **2 → 5 → 7 → 4 → 2 → 9 → 2**. Built mechanical intensity **5 → 5 → 6 → 5 → 2 → 8 → 2**.

The guided path crossed **52 rooms and 1,440 tiles**. Optional room content was **32%** (23 of 73). The five sectors contained **8 independent normal-corridor cycles**. Every authored room has a reachable field reward within **7 tiles** of its centre. A reward is available in every room. The longest guided stretch outside the sensing radius of a field signal or compulsory destination was **2 tiles**.

| Player | Minutes | Marks answered | Wards found | Return rides | Longest quiet stretch | Most room entries between rewards | Climax begins |
|---|---:|---:|---:|---:|---:|---:|---:|
| Solo | 19.40 | 205 | 0 | 0 | 16.4 s | 2 | 95.5% |
| Split | 19.41 | 205 | 0 | 0 | 16.1 s | 2 | 95.5% |
| Rush | 22.10 | 8 | 0 | 0 | 89.6 s | 3 | 95.6% |
| Explore | 25.18 | 257 | 5 | 5 | 20.5 s | 2 | 96.5% |
| Cautious | 20.65 | 213 | 1 | 1 | 20.5 s | 2 | 95.8% |

**Rush.** Passing the signals left this player up to 90 seconds between successes. The field check catches a missed skill before extraction. Answering Reclaim contact in transit avoids a later detour.
**Explore.** Finding all five instances of Spare armature and riding home added 5.8 minutes over the guided run. The mint home marker makes the destination legible before committing.
**Cautious.** The cautious player took the ward loop in the third sector, stunned nearby threats and finished in 20.65 minutes. Lee of the windmill gives the player a safe place to line up the final approach.
**Revision.** The old run let players pass long stretches with a single movement input. The side room added in this revision is Armature bed. A split wreck hull frames the reclaim plate and leaves two ways to reach the winding. This door now teaches Reclaim contact, develops Power winding and asks for all three field responses before the final demonstration. The five new ward rooms connect from both sides. The quickest of these observed policies still had to finish the same three final encounters.

## 24 · One Minute to Midnight

Command & Conquer: Red Alert · 1996 · first Red Alert

Put a powered chrono network in the service of a mail convoy. The last cargo is an invitation and the van can ride with you. Aim for a dramatic change of scale followed by a personal delivery.

Work the coil contacts or take the service road. Visit the post room for a sorting die or keep the van close. Shift the convoy together or escort it to the minute hand on the ground.

**Beats and graph**

Signal yard (2) → Ore exchange (5) → Coil crossing (7) → Chrono pads (4) → Dead-letter office (2) → The minute hand (9) → The last keepsake is a blank place card (2).

Normal edges follow the spine, side passages and ward loop below. The first tool is the key to the chamber shutter. The mint return is a two-way shortcut earned in V. Sector exits are one-way after the required tools, transmission where present, and field practice. The final exit also requires three signature actions after shelter.

| Sector | Spine | Side passages | Ward loop | Route stations | Rooms / tiles | Optional rooms | Cycles | Dead ends and contents |
|---|---|---|---|---|---:|---:|---:|---|
| Signal yard | `upkfghmnot` | `klm no` | `fag` | `km` | 11 / 216 | 4/12 | 2 | t extraction, u arrival landmark |
| Ore exchange | `abchmrqvwx` | `lmn srh` | `hgm` | `hr` | 10 / 212 | 6/14 | 2 | a arrival landmark, l fuel signal, n fuel signal, s fuel signal, x extraction |
| Coil crossing | `ejotsrmlgc` | `ihm qvw rq` | `qpv` | `om` | 10 / 256 | 8/16 | 1 | c extraction, e arrival landmark, i fuel signal, w fuel signal |
| Chrono pads | `klgbdinsxw` | `mno qrs` | `dei` | `lw` | 10 / 406 | 7/15 | 1 | k arrival landmark, m fuel signal, o fuel signal, q fuel signal, w extraction |
| The minute hand | `ytojdchmrw` | `onm qrw` | `him` | `ow` | 11 / 396 | 3/13 | 2 | q fuel signal, w extraction, y arrival landmark |

- **Signal yard** · Start u. Landmark courts upkgmt. Key Coil power at p → k opens the chamber shutter. Fight or aim rooms m Coil grid. Puzzle or traversal rooms k Convoy manifest. Hinted reward a. Shortcut a ↔ u, opened at a. Lock and exit t.
- **Ore exchange** · Start a. Landmark courts abcmqx. Key Convoy credits at b → c opens the chamber shutter. Fight or aim rooms r Coil grid. Puzzle or traversal rooms h Convoy manifest. Hinted reward g. Shortcut g ↔ a, opened at g. Lock and exit x.
- **Coil crossing** · Start e. Landmark courts ejosmc. Key Coil power at j → o opens the chamber shutter. Fight or aim rooms o Coil grid, m Chrono synchroniser. Puzzle or traversal rooms Convoy credits. Hinted reward p. Shortcut p ↔ e, opened at p. Lock and exit c.
- **Chrono pads** · Start k. Landmark courts klginw. Key Mail van at l → g opens the chamber shutter. Fight or aim rooms l Chrono synchroniser. Puzzle or traversal rooms w Convoy manifest. Hinted reward e. Shortcut e ↔ k, opened at e. Lock and exit w.
- **The minute hand** · Start y. Landmark courts ytodmw. Key Mail van at t → o opens the chamber shutter. Fight or aim rooms w Coil grid, m Chrono synchroniser. Puzzle or traversal rooms o Convoy manifest. Hinted reward i. Shortcut i ↔ y, opened at i. Lock and exit w. Rest at Dead-letter office.

**Layout and critical walk**

```text
    Sector 1          Sector 2          Sector 3          Sector 4          Sector 5
aV                aS b* c*                cE    eS       b*    d* eV          c* d*
f* g* h*             gV h*             g. h. i. j*       g*    i*             h* iV j*
k* l. m* n. o.       l. m* n.          l. m*    o*    kS l* m. n* o.          m* n. o*
p*          tE       q* r* s.       pV q. r* s* t*       q. r. s.          q. r*    t*
uS                   v. w. xE          v. w.                wE x.             wE    yS
```

- **Signal yard** · p Coil power → k Coil power finish → k Convoy manifest → m Coil grid → f Convoy credits → g Convoy credits finish → h Mail van → m Mail van finish → t Exit. A switching yard divides the ore train from the mail convoy. Divert power at the centre cabin.
- **Ore exchange** · b Convoy credits → c Convoy credits finish → h Convoy manifest → r Coil grid → h Coil power → m Coil power finish → r Mail van → q Mail van finish → x Exit. The ore exchange wraps around the depot. Bring credits from the terminus to the convoy road.
- **Coil crossing** · j Coil power → o Coil power finish → o Coil grid → m Chrono synchroniser → t Mail van → s Mail van finish → r Convoy credits → m Convoy credits finish → c Exit. A staggered line of coils screens the crossing. Switch the feed to the clock when the convoy passes.
- **Chrono pads** · l Mail van → g Mail van finish → l Chrono synchroniser → w Convoy manifest → b Convoy credits → d Convoy credits finish → i Coil power → n Coil power finish → w Exit. Twin chrono pads occupy opposite ends of the yard. The mail van can skip the central ice cut.
- **The minute hand** · t Mail van → o Mail van finish → j Coil power → d Coil power finish → c Convoy credits → h Convoy credits finish → m Recover dispatch → r Answer dispatch → o Board mail van → (72,43) Dead-letter office → o Convoy manifest → w Coil grid → m Chrono synchroniser → w Exit. Follow the clock perimeter to its winding house. Return the borrowed minute and leave the gate open.

**Content placement**

Dead-letter siding · Two offset loading bays form a through track with a clearly visible sorting platform.

```text
...::..
.XX.:!.
....:..
.:::::.
..:....
.!:.XX.
..::...
```

X is solid cover, : is the approach inlay, ! is an amber warning lamp, and the middle tile holds the optional prize.

- Mail siding appears on travel lines. Players press Use for lift fuel and a brief burst of speed while travelling.
- Chrono contact appears on travel lines. Players pulse on the bright beat for lift fuel and a brief burst of speed while travelling.
- Coil cap appears on travel lines. Players aim and pulse for lift fuel and a brief burst of speed while travelling.
- Coil grid is staged in a widened court. Tune the switchboard and pulse to divert the coil feed. The final sequence revisits this action after the shelter.
- Convoy manifest is staged in a widened court. Use to credit the waiting mail convoy. The final sequence revisits this action after the shelter.
- Chrono synchroniser is staged in a widened court. Pulse while both clocks show the green minute. The final sequence revisits this action after the shelter.
- Sorting die sits between two entrances in the cover shown above. The orbiting amber lamp shows where contact will repel the player. Completing the indicated field skill awards a shared ward and opens the return line at the entrance.
- Dead-letter office sits three tiles from the first finale anchor. It refills both seats and gives the party control over when to begin.

**Verification and playtest**

Planned intensity **2 → 5 → 7 → 4 → 2 → 9 → 2**. Built mechanical intensity **5 → 5 → 7 → 6 → 2 → 9 → 2**.

The guided path crossed **52 rooms and 1,486 tiles**. Optional room content was **40%** (28 of 70). The five sectors contained **8 independent normal-corridor cycles**. Every authored room has a reachable field reward within **7 tiles** of its centre. A reward is available in every room. The longest guided stretch outside the sensing radius of a field signal or compulsory destination was **0 tiles**.

| Player | Minutes | Marks answered | Wards found | Return rides | Longest quiet stretch | Most room entries between rewards | Climax begins |
|---|---:|---:|---:|---:|---:|---:|---:|
| Solo | 20.25 | 211 | 0 | 0 | 15.6 s | 2 | 91.5% |
| Split | 20.25 | 211 | 0 | 0 | 15.6 s | 2 | 91.5% |
| Rush | 22.97 | 8 | 0 | 0 | 121.3 s | 4 | 91.7% |
| Explore | 25.97 | 258 | 5 | 5 | 19.7 s | 2 | 93.5% |
| Cautious | 21.73 | 224 | 1 | 1 | 15.6 s | 2 | 92.1% |

**Rush.** Passing the signals left this player up to 121 seconds between successes. The field check catches a missed skill before extraction. Answering Mail siding in transit avoids a later detour.
**Explore.** Finding all five instances of Sorting die and riding home added 5.7 minutes over the guided run. The mint home marker makes the destination legible before committing.
**Cautious.** The cautious player took the ward loop in the third sector, stunned nearby threats and finished in 21.73 minutes. Dead-letter office gives the player a safe place to line up the final approach.
**Revision.** The old run let players pass long stretches with a single movement input. The side room added in this revision is Dead-letter siding. Two offset loading bays form a through track with a clearly visible sorting platform. This door now teaches Mail siding, develops Chrono contact and asks for all three field responses before the final demonstration. The five new ward rooms connect from both sides. The quickest of these observed policies still had to finish the same three final encounters.
