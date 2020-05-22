# ConquerAll

An attempt at a relatively generic version of a "conquer the world"
type game that emphasizes the development of artificial intelligence
players that can play on any map with an arbitrary combination of rules.

Rules can include what happens when turning in sets -- do you get a
constant aount of new armies depending on the set type, or an escalating
value?  Or maybe it will just nuke the territories instead?

For reinforcements, rules can allow moving only to an adjacent country
or through your territories to a distant land.

More to come.

## Status

It compiles.  Not expected to be playable or even really runnable.
Needs lots of testing.

As of 5/21/20, the game logic seems to work and the RandomAiPlayer also works,
and games have been successfully simulated!

Then the real fun - the real AI players.  It is intended to implement AI
that can intelligently decide on different goals -- do I want to take a bonus
region, take another player out, or ... ?  Then it will have to figure out
a path to do that.

The idea is to be able to simulate millions of games with different AI parameters
and see which ones work best.

Human playability against the AI will hopefully come at some point.


