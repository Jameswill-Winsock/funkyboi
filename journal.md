# Guerilla speedrun hackathon - my experience

19/08/2026 and 20/08/2026 IST

Genuine banger of an event. I did spend so many hours holed up like a hobbit routing pcb like a mad man, but hey, its pretty cool.

Originally, I wanted to do an 8088 carrier motherboard, using RP2040 as carriers, but when I started doing that....... it need like 8 different level shifters and two other ICs for handling the bus line.
<img width="897" height="689" alt="image" src="https://github.com/user-attachments/assets/5c9b9582-239e-407e-aafd-b2a934240e63" />

yeahhhhhhh............... aint nobody handling this in two days. this stuff will atleast take me a week. 

<img width="1084" height="669" alt="image" src="https://github.com/user-attachments/assets/031b4d48-3db4-4362-a657-fc8f6787ceed" />
here you can witness the glorious ratsnest that i still havent wired up. 

So I switched to an idea of mine recently to make a small handheld gameboy/gba clone. Admittedly, its not as small as i expected it to be.... but that comes with time, and this was just 48 hours xd.

What I did first was to start the schematic, and build stuff as normal. That took no time. (i mean, looking up references from other boards does make your job very easy) The issue came when I started designing the PCB. I first decided I would go with a 2 layer pcb - despite clearly needing a 4 layer and spent time trying to jump crossing traces using 0 ohm resistors.
Clearly, that did not last long. So here I am, building my first four layer pcb. Interesting idea to put an entire layer full of copper as ground, and another just for handling your power. Guess you learn something new every day.

Then I ran into another thing. Apparently, you're not supposed to wire your traces hodge podge with mismatched trace lengths and instead keep them as close and symmetric as you can. Not so surprising, but the first time it hits like a truck.
Finally, I finished the difficult part of power delivery, USB, SWD, and the main RP2350B. In all that fun..... I forgot i had to actually build a handheld game device, not just a cool carrier board for the pico. Hence the chonky and funky boi.

I do have to say though, jumping layers using traces is more intuitive, even more so in 4 layer than 2, partly because you see the use of it so much more than in 2. Much useful. 

The reason to leave pin headers littered all over the place is simple: I want to leave the ability to expand this board later in the future using daughterboards, hence the free space next to the pcb for mounting a board later if needed be.
Had to end up using 1.27mm ones because 2.54mm would not fit. I made the core system too small lol.

<img width="947" height="679" alt="image" src="https://github.com/user-attachments/assets/3f8d6504-641c-4d9d-bbfa-7edb8d1214a2" />
i am pretty proud of this one, 'tis nice

The next part will be to make this as concise as possible, and live up to its true name of a funky micro boy and less chonky.

Total hours spent: approx 10 hours.
 
