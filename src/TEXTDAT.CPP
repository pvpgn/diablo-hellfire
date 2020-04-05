/*-----------------------------------------------------------------------**
** 								Diablo
**
**						 Miniquest Text file
**
** 				(C)1995 Condor, Inc. All rights reserved.
**
**-----------------------------------------------------------------------**
**
**						File Routines
**-----------------------------------------------------------------------*/

#include	"diablo.h"
#pragma hdrstop
#include	"minitext.h"
#include	"textdat.h"
#include	"effects.h"

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------**
** Skeleton King Quest
**-----------------------------------------------------------------------*/
#if !IS_VERSION(SHAREWARE)
static char King1[] = " Ahh, the story of our King, is it?  The tragic fall of "
"Leoric was a harsh blow to this land.  The people always loved the King, "
"and now they live in mortal fear of him.  The question that I keep asking "
"myself is how he could have fallen so far from the Light, as Leoric had "
"always been the holiest of men.  Only the vilest powers of Hell could so "
"utterly destroy a man from within... |";

//quest init
static char King2[] = "The village needs your help, good master!  Some months "
"ago King Leoric's son, Prince Albrecht, was kidnapped.  The King went into "
"a rage and scoured the village for his missing child.  With each passing "
"day, Leoric seemed to slip deeper into madness.  He sought to blame "
"innocent townsfolk for the boy's disappearance and had them brutally "
"executed.  Less than half of us survived his insanity...\n \n"
"The King's Knights and Priests tried to placate him, but he turned "
"against them and sadly, they were forced to kill him.  With his dying "
"breath the King called down a terrible curse upon his former followers.  "
"He vowed that they would serve him in darkness forever...\n \n"
"This is where things take an even darker twist than I thought possible!  "
"Our former King has risen from his eternal sleep and now commands a legion "
"of undead minions within the Labyrinth.  His body was buried in a tomb "
"three levels beneath the Cathedral.  Please, good master, put his soul "
"at ease by destroying his now cursed form... |";

//If the hero returns before completing the quest
static char King3[] = "As I told you, good master, the King was entombed "
"three levels below.  He's down there, waiting in the putrid darkness "
"for his chance to destroy this land... |";

//If the hero returns after completing the quest
static char King4[] = "The curse of our King has passed, but I fear that it "
"was only part of a greater evil at work.  However, we may yet be saved "
"from the darkness that consumes our land, for your victory is a good "
"omen.  May Light guide you on your way, good master. |";

static char King5[] = "The loss of his son was too much for King Leoric.  I "
"did what I could to ease his madness, but in the end it overcame him.  "
"A black curse has hung over this kingdom from that day forward, but "
"perhaps if you were to free his spirit from his earthly prison, the "
"curse would be lifted... |";

static char King6[] = "I don't like to think about how the King died.  I "
"like to remember him for the kind and just ruler that he was.  His death "
"was so sad and seemed very wrong, somehow. |";

static char King7[] = "I made many of the weapons and most of the armor "
"that King Leoric used to outfit his knights.  I even crafted a huge "
"two-handed sword of the finest mithril for him, as well as a field "
"crown to match.  I still cannot believe how he died, but it must have "
"been some sinister force that drove him insane! |";

static char King8[] = "I don't care about that.  Listen, no skeleton is gonna "
"be MY king.  Leoric is King.  King, so you hear me?  HAIL TO THE KING! |";

static char King9[] = "The dead who walk among the living follow the cursed "
"King.  He holds the power to raise yet more warriors for an ever growing "
"army of the undead.  If you do not stop his reign, he will surely march "
"across this land and slay all who still live here. |";

static char King10[] = "Look, I'm running a business here.  I don't sell "
"information, and I don't care about some King that's been dead longer "
"than I've been alive.  If you need something to use against this King "
"of the undead, then I can help you out... |";

//When the hero enters the Skeleton King Chamber
static char King11[] = "The warmth of life has entered my tomb.  Prepare "
"yourself, mortal, to serve my Master for eternity! |";
#endif

/*-----------------------------------------------------------------------**
** Banner of Light Quest
**-----------------------------------------------------------------------*/
#if !IS_VERSION(SHAREWARE)
static char Banner1[] = "I see that this strange behavior puzzles you as "
"well.  I would surmise that since many demons fear the light of the sun "
"and believe that it holds great power, it may be that the rising sun "
"depicted on the sign you speak of has led them to believe that it too holds "
"some arcane powers.  Hmm, perhaps they are not all as smart as we had "
"feared... |";

//quest init
static char Banner2[] = "Master, I have a strange experience to relate.  "
"I know that you have a great knowledge of those monstrosities that "
"inhabit the labyrinth, and this is something that I cannot understand "
"for the very life of me... "
"I was awakened during the night by a scraping sound just outside of "
"my tavern.  When I looked out from my bedroom, I saw the shapes of "
"small demon-like creatures in the inn yard.  After a short time, "
"they ran off, but not before stealing the sign to my inn.  I don't "
"know why the demons would steal my sign but leave my family "
"in peace... 'tis strange, no? |";

//If hero returns the sign to the tavern
static char Banner3[] = "Oh, you didn't have to bring back my sign, but I "
"suppose that it does save me the expense of having another one made.  Well, let me see, "
"what could I give you as a fee for finding it?  Hmmm, what have we "
"here... ah, yes!  This cap was left in one of the rooms by a magician "
"who stayed here some time ago.  Perhaps it may be of some value to you. |";

static char Banner4[] = "My goodness, demons running about the village at "
"night, pillaging our homes - is nothing sacred?  I hope that Ogden and "
"Garda are all right.  I suppose that they would come to see me if they "
"were hurt... |";

static char Banner5[] = "Oh my!  Is that where the sign went?  My "
"Grandmother and I must have slept right through the whole thing.  Thank "
"the Light that those monsters didn't attack the inn. |";

static char Banner6[] = "Demons stole Ogden's sign, you say?  That doesn't "
"sound much like the atrocities I've heard of - or seen. \n \n"
"Demons are "
"concerned with ripping out your heart, not your signpost. |";

static char Banner7[] = "You know what I think?  Somebody took that sign, "
"and they gonna want lots of money for it.  If I was Ogden... and I'm not, "
"but if I was... I'd just buy a new sign with some pretty drawing on it.  "
"Maybe a nice mug of ale or a piece of cheese... |";

static char Banner8[] = "No mortal can truly understand the mind of the demon.  \n \n"
"Never let their erratic actions confuse you, as that too may be their "
"plan. |";

static char Banner9[] = "What - is he saying I took that?  I suppose that "
"Griswold is on his side, too. \n \n"
"Look, I got over simple sign stealing "
"months ago.  You can't turn a profit on a piece of wood. |";

//Within the labyrinth
static char Banner10[] = "Hey - You that one that kill all!  You get me "
"Magic Banner or we attack!  You no leave with life!  You kill big uglies "
"and give back Magic.  Go past corner and door, find uglies.  You give, "
"you go! |";

//If hero returns empty-handed
static char Banner11[] = "You kill uglies, get banner.  You bring to me, "
"or else... |";

//If hero returns with sign
static char Banner12[] = "You give!  Yes, good!  Go now, we strong.  We kill "
"all with big Magic! |";
#endif

/*-----------------------------------------------------------------------**
** Vile Betrayer Quest
**-----------------------------------------------------------------------*/
#if !IS_VERSION(SHAREWARE)
// quest init
static char Vile1[] = "This does not bode well, for it confirms my "
"darkest fears.  While I did not allow myself to believe the ancient "
"legends, I cannot deny them now.  Perhaps the time has come to reveal "
"who I am.\n \n"
"My true name is Deckard Cain the Elder, and I am the last descendant "
"of an ancient Brotherhood that was dedicated to safeguarding the secrets "
"of a timeless evil.  An evil that quite obviously has now been released.\n \n"
"The Archbishop Lazarus, once King Leoric's most trusted advisor, led a "
"party of simple townsfolk into the Labyrinth to find the King's missing "
"son, Albrecht.  Quite some time passed before they returned, and only a "
"few of them escaped with their lives.\n \n"
"Curse me for a fool!  I should have suspected his veiled treachery then.  "
"It must have been Lazarus himself who kidnapped Albrecht and has since "
"hidden him within the Labyrinth.  I do not understand why the Archbishop "
"turned to the darkness, or what his interest is in the child, unless he "
"means to sacrifice him to his dark masters!\n \n"
"That must be what he has planned!  The survivors of his 'rescue party' "
"say that Lazarus was last seen running into the deepest bowels of the "
"labyrinth.  You must hurry and save the prince from the sacrificial blade "
"of this demented fiend! |";

// quest not done
static char Vile2[] = "You must hurry and rescue Albrecht from the hands "
"of Lazarus.  The prince and the people of this kingdom are counting on you! |";

// quest done
static char Vile3[] = "Your story is quite grim, my friend.  Lazarus will "
"surely burn in Hell for his horrific deed.  The boy that you describe is "
"not our prince, but I believe that Albrecht may yet be in danger.  The "
"symbol of power that you speak of must be a portal in the very heart of "
"the labyrinth.\n \n"
"Know this, my friend - The evil that you move against is the dark Lord "
"of Terror.  He is known to mortal men as Diablo.  It was he who was "
"imprisoned within the Labyrinth many centuries ago and I fear that he "
"seeks to once again sow chaos in the realm of mankind.  You must venture "
"through the portal and destroy Diablo before it is too late! |";

static char Vile4[] = "Lazarus was the Archbishop who led many of the "
"townspeople into the labyrinth.  I lost many good friends that day, "
"and Lazarus never returned.  I suppose he was killed along with most "
"of the others.  If you would do me a favor, good master - please do "
"not talk to Farnham about that day. |";

static char Vile5[] = "|";

static char Vile6[] = "|";

static char Vile7[] = "I was shocked when I heard of what the townspeople "
"were planning to do that night.  I thought that of all people, Lazarus "
"would have had more sense than that.  He was an Archbishop, and always "
"seemed to care so much for the townsfolk of Tristram.  So many were "
"injured, I could not save them all... |";

static char Vile8[] = "I remember Lazarus as being a very kind and giving "
"man.  He spoke at my mother's funeral, and was supportive of my "
"grandmother and myself in a very troubled time.  I pray every night "
"that somehow, he is still alive and safe. |";

static char Vile9[] = "I was there when Lazarus led us into the labyrinth.  "
"He spoke of holy retribution, but when we started fighting those "
"hellspawn, he did not so much as lift his mace against them.  He just "
"ran deeper into the dim, endless chambers that were filled with the "
"servants of darkness! |";

static char Vile10[] = "They stab, then bite, then they're all around you.  "
"Liar!  LIAR!  They're all dead!  Dead!  Do you hear me?  They just keep "
"falling and falling... their blood spilling out all over the floor... "
"all his fault... |";

static char Vile11[] = "I did not know this Lazarus of whom you speak, but "
"I do sense a great conflict within his being.  He poses a great danger, "
"and will stop at nothing to serve the powers of darkness which have "
"claimed him as theirs. |";

static char Vile12[] = "Yes, the righteous Lazarus, who was sooo effective "
"against those monsters down there.  Didn't help save my leg, did it?  "
"Look, I'll give you a free piece of advice.  Ask Farnham, he was there. |";

//If the hero meets Lazarus after the sacrifice
static char Vile13[] = "Abandon your foolish quest.  All that awaits you "
"is the wrath of my Master!  You are too late to save the child.  Now you will "
"join him in Hell! |";

//deleted
static char Vile14[] = " |";
#endif

/*-----------------------------------------------------------------------**
** Poisoned Water Quest
**-----------------------------------------------------------------------*/
#if !IS_VERSION(SHAREWARE)
static char Poison1[] = "Hmm, I don't know what I can really tell you about "
"this that will be of any help.  The water that fills our wells comes "
"from an underground spring.  I have heard of a tunnel that leads to a "
"great lake - perhaps they are one and the same.  Unfortunately, I do not "
"know what would cause our water supply to be tainted. |";

static char Poison2[] = "I have always tried to keep a large supply of "
"foodstuffs and drink in our storage cellar, but with the entire town "
"having no source of fresh water, even our stores will soon run dry. \n \n"
"Please, do what you can or I don't know what we will do. |";

static char Poison3[] = "I'm glad I caught up to you in time!  Our wells "
"have become brackish and stagnant and some of the townspeople have "
"become ill drinking from them.  Our reserves of fresh water are "
"quickly running dry.  I believe that there is a passage that leads to "
"the springs that serve our town.  Please find what has caused this "
"calamity, or we all will surely perish. |";

//If the hero returns before completing the quest
static char Poison4[] = "Please, you must hurry.  Every hour that passes "
"brings us closer to having no water to drink. \n \n"
"We cannot survive for "
"long without your help. |";

//If the hero completes the quest
static char Poison5[] = "What's that you say - the mere presence of the "
"demons had caused the water to become tainted?  Oh, truly a great evil "
"lurks beneath our town, but your perseverance and courage gives us "
"hope.  Please take this ring - perhaps it will aid you in the "
"destruction of such vile creatures. |";

static char Poison6[] = "My grandmother is very weak, and Garda says "
"that we cannot drink the water from the wells.  Please, can you do something "
"to help us? |";

static char Poison7[] = "Pepin has told you the truth.  We will need fresh "
"water badly, and soon.  I have tried to clear one of the smaller wells, "
"but it reeks of stagnant filth.  It must be getting clogged at the source. |";

static char Poison8[] = "You drink water? |";

static char Poison9[] = "The people of Tristram will die if you cannot "
"restore fresh water to their wells. \n \n"
"Know this - demons are at the heart "
"of this matter, but they remain ignorant of what they have spawned. |";

static char Poison10[] = "For once, I'm with you.  My business runs dry - so "
"to speak - if I have no market to sell to.  You better find out what is "
"going on, and soon! |";
#endif

/*-----------------------------------------------------------------------**
** The Chamber of Bone Quest
**-----------------------------------------------------------------------*/
#if !IS_VERSION(SHAREWARE)
static char Bone1[] = "A book that speaks of a chamber of human bones?  Well, "
"a Chamber of Bone is mentioned in certain archaic writings that I "
"studied in the libraries of the East.  These tomes inferred that when "
"the Lords of the underworld desired to protect great treasures, they "
"would create domains where those who died in the attempt to steal that "
"treasure would be forever bound to defend it.  A twisted, but strangely "
"fitting, end? |";

static char Bone2[] = "I am afraid that I don't know anything about that, "
"good master.  Cain has many books that may be of some help. |";

static char Bone3[] = "This sounds like a very dangerous place.  If you "
"venture there, please take great care. |";

static char Bone4[] = "I am afraid that I haven't heard anything about "
"that.  Perhaps Cain the Storyteller could be of some help. |";

static char Bone5[] = "I know nothing of this place, but you may try "
"asking Cain.  He talks about many things, and it would not surprise "
"me if he had some answers to your question. |";

static char Bone6[] = "Okay, so listen.  There's this chamber of wood, "
"see.  And his wife, you know - her - tells the tree... cause you gotta "
"wait.  Then I says, that might work against him, but if you think I'm "
"gonna PAY for this... you... uh... yeah. |";

static char Bone7[] = "You will become an eternal servant of the dark "
"lords should you perish within this cursed domain. \n \n"
"Enter the Chamber "
"of Bone at your own peril. |";

static char Bone8[] = "A vast and mysterious treasure, you say?  Maybe I "
"could be interested in picking up a few things from you... or better "
"yet, don't you need some rare and expensive supplies to get you through "
"this ordeal? |";
#endif

/*-----------------------------------------------------------------------**
** The Butcher Quest
**-----------------------------------------------------------------------*/
#if !IS_VERSION(SHAREWARE)
static char Butch1[] = "It seems that the Archbishop Lazarus goaded many "
"of the townsmen into venturing into the Labyrinth to find the King's "
"missing son.  He played upon their fears and whipped them into a "
"frenzied mob.  None of them were prepared for what lay within the cold "
"earth... Lazarus abandoned them down there - left in the clutches of "
"unspeakable horrors - to die. |";

static char Butch2[] = "Yes, Farnham has mumbled something about a hulking "
"brute who wielded a fierce weapon.  I believe he called him a butcher. |";

static char Butch3[] = "By the Light, I know of this vile demon.  There were "
"many that bore the scars of his wrath upon their bodies when the few "
"survivors of the charge led by Lazarus crawled from the Cathedral.  I "
"don't know what he used to slice open his victims, but it could not "
"have been of this world.  It left wounds festering with disease and "
"even I found them almost impossible to treat.  Beware if you plan to "
"battle this fiend... |";

static char Butch4[] = "When Farnham said something about a butcher killing "
"people, I immediately discounted it.  But since you brought it up, maybe "
"it is true. |";

static char Butch5[] = "I saw what Farnham calls the Butcher as it swathed "
"a path through the bodies of my friends.  He swung a cleaver as large "
"as an axe, hewing limbs and cutting down brave men where they stood.  I "
"was separated from the fray by a host of small screeching demons and "
"somehow found the stairway leading out.  I never saw that hideous beast "
"again, but his blood-stained visage haunts me to this day. |";

static char Butch6[] = "Big!  Big cleaver killing all my friends.  Couldn't "
"stop him, had to run away, couldn't save them.  Trapped in a room with "
"so many bodies... so many friends... NOOOOOOOOOO! |";

static char Butch7[] = "The Butcher is a sadistic creature that delights "
"in the torture and pain of others.  You have seen his handiwork in the "
"drunkard Farnham.  His destruction will do much to ensure the safety of "
"this village. |";

static char Butch8[] = "I know more than you'd think about that grisly "
"fiend.  His little friends got a hold of me and managed to get my leg "
"before Griswold pulled me out of that hole. \n \n"
"I'll put it bluntly - "
"kill him before he kills you and adds your corpse to his collection. |";

//wounded townsman
static char Butch9[] = "Please, listen to me.  The Archbishop "
"Lazarus, he led us down here to find the lost prince.  The bastard "
"led us into a trap!  Now everyone is dead... killed by a demon he "
"called the Butcher.  Avenge us!  Find this Butcher and slay him so "
"that our souls may finally rest... |";

//butcher
static char Butch10[] = " |";
#endif

/*-----------------------------------------------------------------------**
** Halls of the Blind Quest
**-----------------------------------------------------------------------*/
#if !IS_VERSION(SHAREWARE)
static char Blind1[] = "You recite an interesting rhyme written in a style "
"that reminds me of other works.  Let me think now - what was it?\n \n"
"...Darkness shrouds the Hidden.  Eyes glowing unseen with only the "
"sounds of razor claws briefly scraping to torment those poor souls who "
"have been made sightless for all eternity.  The prison for those so "
"damned is named the Halls of the Blind... |";

static char Blind2[] = "I never much cared for poetry.  Occasionally, I had "
"cause to hire minstrels when the inn was doing well, but that seems "
"like such a long time ago now. \n \n"
"What?  Oh, yes... uh, well, I suppose you "
"could see what someone else knows. |";

static char Blind3[] = "This does seem familiar, somehow.  I seem to recall "
"reading something very much like that poem while researching the "
"history of demonic afflictions.  It spoke of a place of great evil "
"that... wait - you're not going there are you? |";

static char Blind4[] = "If you have questions about blindness, you should "
"talk to Pepin.  I know that he gave my grandmother a potion that helped "
"clear her vision, so maybe he can help you, too. |";

static char Blind5[] = "I am afraid that I have neither heard nor seen a "
"place that matches your vivid description, my friend.  Perhaps Cain "
"the Storyteller could be of some help. |";

static char Blind6[] = "Look here... that's pretty funny, huh? "
"Get it?  Blind - look here? |";

static char Blind7[] = "This is a place of great anguish and terror, and "
"so serves its master well. \n \n"
"Tread carefully or you may yourself "
"be staying much longer than you had anticipated. |";

static char Blind8[] = "Lets see, am I selling you something?  No.  Are you "
"giving me money to tell you about this?  No.  Are you now leaving and "
"going to talk to the storyteller who lives for this kind of thing?  "
"Yes. |";
#endif

/*-----------------------------------------------------------------------**
** Veil of Steel Quest
**-----------------------------------------------------------------------*/
#if !IS_VERSION(SHAREWARE)
static char Veil1[] = "You claim to have spoken with Lachdanan?  He was a "
"great hero during his life.  Lachdanan was an honorable and just man "
"who served his King faithfully for years.  But of course, you already "
"know that.\n \n"
"Of those who were caught within the grasp of the King's "
"Curse, Lachdanan would be the least likely to submit to the darkness "
"without a fight, so I suppose that your story could be true.  If I were "
"in your place, my friend, I would find a way to release him from his "
"torture. |";

static char Veil2[] = "You speak of a brave warrior long dead!  I'll have "
"no such talk of speaking with departed souls in my inn yard, thank you "
"very much. |";

static char Veil3[] = "A golden elixir, you say.  I have never concocted a "
"potion of that color before, so I can't tell you how it would effect "
"you if you were to try to drink it.  As your healer, I strongly advise "
"that should you find such an elixir, do as Lachdanan asks and "
"DO NOT try to use it. |";

static char Veil4[] = "I've never heard of a Lachdanan before.  I'm sorry, but "
"I don't think that I can be of much help to you. |";

static char Veil5[] = "If it is actually Lachdanan that you have met, then "
"I would advise that you aid him.  I dealt with him on several occasions "
"and found him to be honest and loyal in nature.  The curse that fell "
"upon the followers of King Leoric would fall especially hard upon him. |";

static char Veil6[] = " Lachdanan is dead.  Everybody knows that, and you "
"can't fool me into thinking any other way.  You can't talk to the dead.  "
"I know! |";

static char Veil7[] = "You may meet people who are trapped within the "
"Labyrinth, such as Lachdanan. \n \n"
"I sense in him honor and great guilt.  "
"Aid him, and you aid all of Tristram. |";

static char Veil8[] = "Wait, let me guess.  Cain was swallowed up in a "
"gigantic fissure that opened beneath him.  He was incinerated in a ball "
"of hellfire, and can't answer your questions anymore.  Oh, that isn't "
"what happened?  Then I guess you'll be buying something or you'll be "
"on your way. |";

//upon meeting the captain
static char Veil9[] = "Please, don't kill me, just hear me out.  I was once "
"Captain of King Leoric's Knights, upholding the laws of this land with "
"justice and honor.  Then his dark Curse fell upon us for the role we "
"played in his tragic death.  As my fellow Knights succumbed to their "
"twisted fate, I fled from the King's burial chamber, searching for "
"some way to free myself from the Curse.  I failed...\n \n"
"I have heard of a Golden Elixir that could lift the Curse and allow "
"my soul to rest, but I have been unable to find it.  My strength now "
"wanes, and with it the last of my humanity as well.  Please aid me and "
"find the Elixir.  I will repay your efforts - I swear upon my honor. |";

//If the hero returns without the Elixir
static char Veil10[] = "You have not found the Golden Elixir.  I fear that "
"I am doomed for eternity.  Please, keep trying... |";

//If the hero returns with the Elixir
static char Veil11[] = "You have saved my soul from damnation, and for "
"that I am in your debt.  If there is ever a way that I can repay you "
"from beyond the grave I will find it, but for now - take my helm.  On "
"the journey I am about to take I will have little use for it.  May it "
"protect you against the dark powers below.  Go with the Light, my "
"friend... |";
#endif

/*-----------------------------------------------------------------------**
** The Anvil of Fury Quest
**-----------------------------------------------------------------------*/
#if !IS_VERSION(SHAREWARE)
static char Anvil1[] = "Griswold speaks of The Anvil of Fury - a legendary "
"artifact long searched for, but never found.  Crafted from the metallic "
"bones of the Razor Pit demons, the Anvil of Fury was smelt around the "
"skulls of the five most powerful magi of the underworld.  Carved with "
"runes of power and chaos, any weapon or armor forged upon this Anvil "
"will be immersed into the realm of Chaos, imbedding it with magical "
"properties.  It is said that the unpredictable nature of Chaos makes "
"it difficult to know what the outcome of this smithing will be... |";

static char Anvil2[] = "Don't you think that Griswold would be a better "
"person to ask about this?  He's quite handy, you know. |";

static char Anvil3[] = "If you had been looking for information on the "
"Pestle of Curing or the Silver Chalice of Purification, I could have "
"assisted you, my friend.  However, in this matter, you would be better "
"served to speak to either Griswold or Cain. |";

static char Anvil4[] = "Griswold's father used to tell some of us when we "
"were growing up about a giant anvil that was used to make mighty "
"weapons.  He said that when a hammer was struck upon this anvil, the "
"ground would shake with a great fury.  Whenever the earth moves, I "
"always remember that story. |";

static char Anvil5[] = "Greetings!  It's always a pleasure to see one of "
"my best customers!  I know that you have been venturing deeper into "
"the Labyrinth, and there is a story I was told that you may find worth "
"the time to listen to...\n \n"
"One of the men who returned from the Labyrinth told me about a mystic "
"anvil that he came across during his escape.  His description reminded "
"me of legends I had heard in my youth about the burning Hellforge where "
"powerful weapons of magic are crafted.  The legend had it that deep "
"within the Hellforge rested the Anvil of Fury!  This Anvil contained "
"within it the very essence of the demonic underworld...\n \n"
"It is said that any weapon crafted upon the burning Anvil is imbued "
"with great power.  If this anvil is indeed the Anvil of Fury, I may "
"be able to make you a weapon capable of defeating even the darkest "
"lord of Hell! \n \n"
"Find the Anvil for me, and I'll get to work! |";

//If the hero returns without the Anvil
static char Anvil6[] = "Nothing yet, eh?  Well, keep searching.  A weapon "
"forged upon the Anvil could be your best hope, and I am sure that I "
"can make you one of legendary proportions. |";

//If the hero returns with the Anvil
static char Anvil7[] = "I can hardly believe it!  This is the Anvil of Fury "
"- good work, my friend.  Now we'll show those bastards that there are "
"no weapons in Hell more deadly than those made by men!  Take this and "
"may Light protect you. |";

static char Anvil8[] = "Griswold can't sell his anvil.  What will he do "
"then?  And I'd be angry too if someone took my anvil! |";

static char Anvil9[] = "There are many artifacts within the Labyrinth "
"that hold powers beyond the comprehension of mortals.  Some of these "
"hold fantastic power that can be used by either the Light or the "
"Darkness.  Securing the Anvil from below could shift the course of "
"the Sin War towards the Light. |";

static char Anvil10[] = "If you were to find this artifact for Griswold, "
"it could put a serious damper on my business here.  Awwww, you'll never "
"find it. |";
#endif

/*-----------------------------------------------------------------------**
** Stones of Blood Quest
**-----------------------------------------------------------------------*/
#if !IS_VERSION(SHAREWARE)
static char Blood1[] = "The Gateway of Blood and the Halls of Fire are "
"landmarks of mystic origin.  Wherever this book you read from resides it is "
"surely a place of great power.\n \n"
"Legends speak of a pedestal that is carved from obsidian stone and "
"has a pool of boiling blood atop its bone encrusted surface.  There "
"are also allusions to Stones of Blood that will open a door that "
"guards an ancient treasure...\n \n"
"The nature of this treasure is shrouded in speculation, my friend, "
"but it is said that the ancient hero Arkaine placed the holy armor "
"Valor in a secret vault.  Arkaine was the first mortal to turn the "
"tide of the Sin War and chase the legions of darkness back to the "
"Burning Hells.\n \n" 
"Just before Arkaine died, his armor was hidden away "
"in a secret vault.  It is said that when this holy armor is again "
"needed, a hero will arise to don Valor once more.  Perhaps you are "
"that hero... |";

static char Blood2[] = "Every child hears the story of the warrior Arkaine "
"and his mystic armor known as Valor.  If you could find its resting "
"place, you would be well protected against the evil in the "
"Labyrinth. |";

static char Blood3[] = "Hmm... it sounds like something I should remember, "
"but I've been so busy learning new cures and creating better elixirs "
"that I must have forgotten.  Sorry... |";

static char Blood4[] = "The story of the magic armor called Valor is "
"something I often heard the boys talk about.  You had better ask one "
"of the men in the village. |";

static char Blood5[] = "The armor known as Valor could be what tips the "
"scales in your favor.  I will tell you that many have looked for it - "
"including myself.  Arkaine hid it well, my friend, and it will take "
"more than a bit of luck to unlock the secrets that have kept it "
"concealed oh, lo these many years. |";

static char Blood6[] = "Zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz... |";

static char Blood7[] = "Should you find these Stones of Blood, use them "
"carefully. \n \n"
"The way is fraught with danger and your only hope rests "
"within your self trust. |";

static char Blood8[] = "You intend to find the armor known as Valor? \n \n"
"No "
"one has ever figured out where Arkaine stashed the stuff, and if my "
"contacts couldn't find it, I seriously doubt you ever will either. |";
#endif

/*-----------------------------------------------------------------------**
** Warlord of Blood Quest
**-----------------------------------------------------------------------*/
#if !IS_VERSION(SHAREWARE)
static char Warlrd1[] = "I know of only one legend that speaks of such a "
"warrior as you describe.  His story is found within the ancient "
"chronicles of the Sin War...\n \n"
"Stained by a thousand years of war, blood and death, the Warlord of "
"Blood stands upon a mountain of his tattered victims.  His dark blade "
"screams a black curse to the living; a tortured invitation to any who "
"would stand before this Executioner of Hell.\n \n"
"It is also written that "
"although he was once a mortal who fought beside the Legion of Darkness "
"during the Sin War, he lost his humanity to his insatiable hunger for "
"blood. |";

static char Warlrd2[] = "I am afraid that I haven't heard anything about "
"such a vicious warrior, good master.  I hope that you do not have to fight "
"him, for he sounds extremely dangerous. |";

static char Warlrd3[] = "Cain would be able to tell you much more about "
"something like this than I would ever wish to know. |";

static char Warlrd4[] = "If you are to battle such a fierce opponent, may "
"Light be your guide and your defender.  I will keep you in my thoughts. |";

static char Warlrd5[] = "Dark and wicked legends surrounds the one Warlord "
"of Blood.  Be well prepared, my friend, for he shows no mercy or quarter. |";

static char Warlrd6[] = "Always you gotta talk about Blood?  What about "
"flowers, and sunshine, and that pretty girl that brings the drinks.  "
"Listen here, friend - you're obsessive, you know that? |";

static char Warlrd7[] = "His prowess with the blade is awesome, and he "
"has lived for thousands of years knowing only warfare.  I am sorry... "
"I can not see if you will defeat him. |";

static char Warlrd8[] = "I haven't ever dealt with this Warlord you speak "
"of, but he sounds like he's going through a lot of swords.  Wouldn't mind "
"supplying his armies... |";

static char Warlrd9[] = "My blade sings for your blood, mortal, and by my "
"dark masters it shall not be denied. |";
#endif

/*-----------------------------------------------------------------------**
** Ring of Infravision Quest
**-----------------------------------------------------------------------*/
#if !IS_VERSION(SHAREWARE)
static char Infra1[] = "Griswold speaks of the Heaven Stone that was "
"destined for the enclave located in the east.  It was being taken "
"there for further study.  This stone glowed with an energy that "
"somehow granted vision beyond that which a normal man could possess.  "
"I do not know what secrets it holds, my friend, but finding this "
"stone would certainly prove most valuable. |";

static char Infra2[] = "The caravan stopped here to take on some supplies "
"for their journey to the east.  I sold them quite an array of fresh "
"fruits and some excellent sweetbreads that Garda has just finished "
"baking.  Shame what happened to them... |";

static char Infra3[] = "I don't know what it is that they thought they "
"could see with that rock, but I will say this.  If rocks are falling "
"from the sky, you had better be careful! |";

static char Infra4[] = "Well, a caravan of some very important people did "
"stop here, but that was quite a while ago.  They had strange accents "
"and were starting on a long journey, as I recall. \n \n"
"I don't see how you "
"could hope to find anything that they would have been carrying. |";

static char Infra5[] = "Stay for a moment - I have a story you might find "
"interesting.  A caravan that was bound for the eastern kingdoms passed "
"through here some time ago.  It was supposedly carrying a piece of the "
"heavens that had fallen to earth!  The caravan was ambushed by cloaked "
"riders just north of here along the roadway.  I searched the wreckage "
"for this sky rock, but it was nowhere to be found.  If you should find "
"it, I believe that I can fashion something useful from it. |";

//If the hero returns before completing the quest
static char Infra6[] = "I am still waiting for you to bring me that stone "
"from the heavens.  I know that I can make something powerful out of it. |";

//If the hero returns before completing the quest
static char Infra7[] = "Let me see that - aye... aye, it is as I believed.  Give me "
"a moment...\n \n"
"Ah, Here you are.  I arranged pieces of the stone within a silver ring "
"that my father left me.  I hope it serves you well. |";

static char Infra8[] = "I used to have a nice ring; it was a really "
"expensive one, with blue and green and red and silver.  Don't remember "
"what happened to it, though.  I really miss that ring... |";

static char Infra9[] = "The Heaven Stone is very powerful, and were it "
"any but Griswold who bid you find it, I would prevent it.  He will "
"harness its powers and its use will be for the good of us all. |";

static char Infra10[] = "If anyone can make something out of that rock, "
"Griswold can.  He knows what he is doing, and as much as I try to "
"steal his customers, I respect the quality of his work. |";
#endif

/*-----------------------------------------------------------------------**
** Black Mushroom Quest
**-----------------------------------------------------------------------*/
#if !IS_VERSION(SHAREWARE)
static char Mush1[] = "The witch Adria seeks a black mushroom?  I know as "
"much about Black Mushrooms as I do about Red Herrings.  Perhaps "
"Pepin the Healer could tell you more, but this is something that "
"cannot be found in any of my stories or books. |";

//Tavern guy
static char Mush2[] = "Let me just say this.  Both Garda and I would never, "
"EVER serve black mushrooms to our honored guests.  If Adria wants some "
"mushrooms in her stew, then that is her business, but I can't help you "
"find any.  Black mushrooms... disgusting! |";

//If the hero goes to the healer without the brain
static char Mush3[] = "The witch told me that you were searching for the "
"brain of a demon to assist me in creating my elixir.  It should be of "
"great value to the many who are injured by those foul beasts, if I can "
"just unlock the secrets I suspect that its alchemy holds.  If you can "
"remove the brain of a demon when you kill it, I would be grateful if "
"you could bring it to me. |";

//If the hero brings a brain to the healer
static char Mush4[] = "Excellent, this is just what I had in mind.  I was "
"able to finish the elixir without this, but it can't hurt to have this "
"to study.  Would you please carry this to the witch?  I believe that she "
"is expecting it. |";

static char Mush5[] = "I think Ogden might have some mushrooms in the "
"storage cellar.  Why don't you ask him? |";

static char Mush6[] = "If Adria doesn't have one of these, you can bet "
"that's a rare thing indeed.  I can offer you no more help than that, "
"but it sounds like... a huge, gargantuan, swollen, bloated mushroom!  " 
"Well, good hunting, I suppose. |";

static char Mush7[] = "Ogden mixes a MEAN black mushroom, but I get sick "
"if I drink that.  Listen, listen... here's the secret - moderation is "
"the key! |";

//If hero brings the Fungal Tome to the Witch
static char Mush8[] = "What do we have here?  Interesting, it looks like a "
"book of reagents.  Keep your eyes open for a black mushroom.  It should "
"be fairly large and easy to identify.  If you find it, bring it to me, "
"won't you? |";

//If hero returns without the mushroom
static char Mush9[] = "It's a big, black mushroom that I need.  Now run off "
"and get it for me so that I can use it for a special concoction that "
"I am working on. |";

//If hero brings the mushroom to the witch
static char Mush10[] = "Yes, this will be perfect for a brew that I am "
"creating.  By the way, the healer is looking for the brain of some "
"demon or another so he can treat those who have been afflicted by "
"their poisonous venom.  I believe that he intends to make an elixir "
"from it.  If you help him find what he needs, please see if you can "
"get a sample of the elixir for me. |";

//If hero brings the brain to the witch
static char Mush11[] = "Why have you brought that here?  I have no need "
"for a demon's brain at this time.  I do need some of the elixir that "
"the Healer is working on.  He needs that grotesque organ that you "
"are holding, and then bring me the elixir.  Simple when you think about "
"it, isn't it? |";

//If the hero takes the elixir to the witch
static char Mush12[] = "What?  Now you bring me that elixir from the healer?  "
"I was able to finish my brew without it.  Why don't you just keep it... |";

static char Mush13[] = "I don't have any mushrooms of any size or color "
"for sale.  How about something a bit more useful? |";
#endif

/*-----------------------------------------------------------------------**
** Map of Doom Quest
**-----------------------------------------------------------------------*/
#if !IS_VERSION(SHAREWARE)
static char Doom1[] = "So, the legend of the Map is real.  Even I never "
"truly believed any of it!  I suppose it is time that I told you the "
"truth about who I am, my friend.  You see, I am not all that I seem...\n \n"
"My true name is Deckard Cain the Elder, and I am the last descendant "
"of an ancient Brotherhood that was dedicated to keeping and safeguarding "
"the secrets of a timeless evil.  An evil that quite obviously has now "
"been released...\n \n"
"The evil that you move against is the dark Lord of Terror - known to "
"mortal men as Diablo.  It was he who was imprisoned within the Labyrinth "
"many centuries ago.  The Map that you hold now was created ages ago to "
"mark the time when Diablo would rise again from his imprisonment.  When "
"the two stars on that map align, Diablo will be at the height of his "
"power.  He will be all but invincible...\n \n"
"You are now in a race against time, my friend!  Find Diablo and destroy "
"him before the stars align, for we may never have a chance to rid the "
"world of his evil again! |";

//If the hero returns before the stars align and Diablo is Alive
static char Doom2[] = "Our time is running short!  I sense his dark power "
"building and only you can stop him from attaining his full might. |";

//If the hero returns after the stars align and Diablo is Alive
static char Doom3[] = "I am sure that you tried your best, but I fear that "
"even your strength and will may not be enough.  Diablo is now at the "
"height of his earthly power, and you will need all your courage and "
"strength to defeat him.  May the Light protect and guide you, my friend.  "
"I will help in any way that I am able. |";

static char Doom4[] = "If the witch can't help you and suggests you see Cain, "
"what makes you think that I would know anything?  It sounds like this is a "
"very serious matter.  You should hurry along and see the storyteller as "
"Adria suggests. |";

static char Doom5[] = "I can't make much of the writing on this map, but "
"perhaps Adria or Cain could help you decipher what this refers to. \n \n"
"I can see that it is a map of the stars in our sky, but any more than "
"that is beyond my talents. |";

static char Doom6[] = "The best person to ask about that sort of thing "
"would be our storyteller. \n \n"
"Cain is very knowledgeable about ancient "
"writings, and that is easily the oldest looking piece of paper that "
"I have ever seen. |";

static char Doom7[] = "I have never seen a map of this sort before.  Where'd you get it?  "
"Although I have no idea how to read this, Cain or Adria may be able "
"to provide the answers that you seek. |";

static char Doom8[] = "Listen here, come close.  I don't know if you know "
"what I know, but you have really got somethin' here.  That's a map. |";

static char Doom9[] = "Oh, I'm afraid this does not bode well at all.  This map "
"of the stars portends great disaster, but its secrets are not mine to "
"tell.  The time has come for you to have a very serious conversation "
"with the Storyteller... |";

static char Doom10[] = "I've been looking for a map, but that certainly "
"isn't it.  You should show that to Adria - she can probably tell you "
"what it is.  I'll say one thing; it looks old, and old usually means "
"valuable. |";
#endif

/*-----------------------------------------------------------------------**
** Garbud the Weak Quest
**-----------------------------------------------------------------------*/
#if !IS_VERSION(SHAREWARE)
//Gharbad the Weak Encounter
static char Garbud1[] = "Pleeeease, no hurt.  No Kill.  Keep alive and next "
"time good bring to you. |";

//When the hero finds Gharbad again
static char Garbud2[] = "Something for you I am making.  Again, not kill "
"Gharbad.  Live and give good. \n \n"
"You take this as proof I keep word... |";

//When the hero finds Gharbad again
static char Garbud3[] = "Nothing yet!  Almost done. \n \n"
"Very powerful, very "
"strong.  Live!  Live! \n \n"
"No pain and promise I keep! |";

//When the hero finds Gharbad again
static char Garbud4[] = "This too good for you.  Very Powerful!  You want - "
"you take! |";
#endif

/*-----------------------------------------------------------------------**
** Zhar the Mad Quest
**-----------------------------------------------------------------------*/
#if !IS_VERSION(SHAREWARE)
//Zhar the Mad Encounter
static char Zhar1[] = "What?!  Why are you here?  All these interruptions "
"are enough to make one insane.  Here, take this and leave me to my work.  "
"Trouble me no more! |";

//If the hero bothers him again
static char Zhar2[] = "Arrrrgh!  Your curiosity will be the death of you!!! |";
#endif

/*-----------------------------------------------------------------------**
//	TOWNSPEOPLE'S NONQUEST
**-----------------------------------------------------------------------*/
//	Deckard Cain
static char Story1[] = "Hello, my friend.  Stay awhile and listen... |";

#if !IS_VERSION(SHAREWARE)
static char Story2[] = "While you are venturing deeper into the Labyrinth "
"you may find tomes of great knowledge hidden there. \n \n"
"Read them carefully "
"for they can tell you things that even I cannot. |";

static char Story3[] = "I know of many myths and legends that may contain "
"answers to questions that may arise in your journeys into the Labyrinth.  "
"If you come across challenges and questions to which you seek knowledge, "
"seek me out and I will tell you what I can. |";

static char Story4[] = "Griswold - a man of great action and great courage.  "
"I bet he never told you about the time he went into the Labyrinth to "
"save Wirt, did he?  He knows his fair share of the dangers to be found "
"there, but then again - so do you.  He is a skilled craftsman, and if "
"he claims to be able to help you in any way, you can count on his "
"honesty and his skill. |";

static char Story5[] = "Ogden has owned and run the Rising Sun Inn and "
"Tavern for almost four years now.  He purchased it just a few short "
"months before everything here went to hell.  He and his wife Garda do "
"not have the money to leave as they invested all they had in making "
"a life for themselves here.  He is a good man with a deep sense of "
"responsibility. |";


static char Story6[] = "Poor Farnham.  He is a disquieting reminder of the "
"doomed assembly that entered into the Cathedral with Lazarus on that "
"dark day.  He escaped with his life, but his courage and much of his "
"sanity were left in some dark pit.  He finds comfort only at the bottom "
"of his tankard nowadays, but there are occasional bits of truth buried "
"within his constant ramblings. |";

static char Story7[] = "The witch, Adria, is an anomaly here in Tristram.  "
"She arrived shortly after the Cathedral was desecrated while most "
"everyone else was fleeing.  She had a small hut constructed at the "
"edge of town, seemingly overnight, and has access to many strange "
"and arcane artifacts and tomes of knowledge that even I have never "
"seen before. |";

static char Story9[] = "The story of Wirt is a frightening and tragic one.  "
"He was taken from the arms of his mother "
"and dragged into the labyrinth by the small, foul demons that wield "
"wicked spears.  There were many other children taken that day, including "
"the son of King Leoric.  The Knights of the palace went below, but "
"never returned.  The Blacksmith found the boy, but only after the foul "
"beasts had begun to torture him for their sadistic pleasures. |";

static char Story10[] = "Ah, Pepin.  I count him as a true friend - perhaps "
"the closest I have here.  He is a bit addled at times, but never a more "
"caring or considerate soul has existed.  His knowledge and skills are "
"equaled by few, and his door is always open. |";

static char Story11[] = "Gillian is a fine woman.  Much adored for her high "
"spirits and her quick laugh, she holds a special place in my heart.  She "
"stays on at the tavern to support her elderly grandmother who is too "
"sick to travel.  I sometimes fear for her safety, but I know that any man "
"in the village would rather die than see her harmed. |";
#endif

//	Ogden
static char Ogden1[] = "Greetings, good master.  Welcome to the Tavern of "
"the Rising Sun! |";

#if !IS_VERSION(SHAREWARE)
static char Ogden2[] = "Many adventurers have graced the tables of my tavern, "
"and ten times as many stories have been told over as much ale.  The only "
"thing that I ever heard any of them agree on was this old axiom.  Perhaps "
"it will help you.  "
"You can cut the flesh, but you must crush the bone. |";

static char Ogden3[] = "Griswold the blacksmith is extremely knowledgeable "
"about weapons and armor.  If you ever need work done on your gear, he is "
"definitely the man to see. |";

static char Ogden4[] = "Farnham spends far too much time here, drowning his "
"sorrows in cheap ale.  I would make him leave, but he did suffer so during "
"his time in the Labyrinth. |";

static char Ogden5[] = "Adria is wise beyond her years, but I must admit - "
"she frightens me a little. \n \n"
"Well, no matter.  If you ever have need to "
"trade in items of sorcery, she maintains a strangely well-stocked hut "
"just across the river. |";

static char Ogden6[] = "If you want to know more about the history of our "
"village, the storyteller Cain knows quite a bit about the past. |";

static char Ogden8[] = "Wirt is a rapscallion and a little scoundrel.  He "
"was always getting into trouble, and it's no surprise what happened to "
"him. \n \n"
"He probably went fooling about someplace that he shouldn't have "
"been.  I feel sorry for the boy, but I don't abide the company that he keeps.  |";

static char Ogden9[] = "Pepin is a good man - and certainly the most "
"generous in the village.  He is always attending to the needs of others, "
"but trouble of some sort or another does seem to follow him wherever he "
"goes... |";

static char Ogden10[] = "Gillian, my Barmaid?  If it were not for her sense "
"of duty to her grand-dam, she would have fled from here long ago. \n \n"
"Goodness knows I begged her to leave, telling her that I would watch "
"after the old woman, but she is too sweet and caring to have done so. |";
#endif

//	Pepin
static char Pepin1[] = "What ails you, my friend? |";

#if !IS_VERSION(SHAREWARE)
static char Pepin2[] = "I have made a very interesting discovery.  Unlike "
"us, the creatures in the Labyrinth can heal themselves without the aid "
"of potions or magic.  If you hurt one of the monsters, make sure it is "
"dead or it very well may regenerate itself. |";

static char Pepin3[] = "Before it was taken over by, well, whatever lurks "
"below, the Cathedral was a place of great learning.  There are many "
"books to be found there.  If you find any, you should read them all, "
"for some may hold secrets to the workings of the Labyrinth. |";

static char Pepin4[] = "Griswold knows as much about the art of war as I "
"do about the art of healing.  He is a shrewd merchant, but his work is "
"second to none.  Oh, I suppose that may be because he is the only "
"blacksmith left here. |";

static char Pepin5[] = "Cain is a true friend and a wise sage.  He maintains "
"a vast library and has an innate ability to discern the true nature of "
"many things.  If you ever have any questions, he is the person to go to. |";

static char Pepin6[] = "Even my skills have been unable to fully heal "
"Farnham.  Oh, I have been able to mend his body, but his mind and spirit "
"are beyond anything I can do. |";

static char Pepin7[] = "While I use some limited forms of magic to create "
"the potions and elixirs I store here, Adria is a true sorceress.  She "
"never seems to sleep, and she always has access to many mystic tomes "
"and artifacts.  I believe her hut may be much more than the hovel "
"it appears to be, but I can never seem to get inside the place. |";

static char Pepin9[] = "Poor Wirt.  I did all that was possible for the "
"child, but I know he despises that wooden peg that I was forced to "
"attach to his leg.  His wounds were hideous.  No one - and especially "
"such a young child - should have to suffer the way he did. |";

static char Pepin10[] = "I really don't understand why Ogden stays here "
"in Tristram.  He suffers from a slight nervous condition, but he is an "
"intelligent and industrious man who would do very well wherever he "
"went.  I suppose it may be the fear of the many murders that happen in "
"the surrounding countryside, or perhaps the wishes of his wife that "
"keep him and his family where they are. |";

static char Pepin11[] = "Ogden's barmaid is a sweet girl.  Her grandmother "
"is quite ill, and suffers from delusions. \n \n"
"She claims that they are "
"visions, but I have no proof of that one way or the other. |";
#endif

//	Gillian
static char Gillian1[] = "Good day!  How may I serve you? |";

#if !IS_VERSION(SHAREWARE)
static char Gillian2[] = "My grandmother had a dream that you would come "
"and talk to me.  She has visions, you know and can see into the future. |";

static char Gillian3[] = "The woman at the edge of town is a witch!  She "
"seems nice enough, and her name, Adria, is very pleasing to the ear, "
"but I am very afraid of her. \n \n"
"It would take someone quite brave, like "
"you, to see what she is doing out there. |";

static char Gillian4[] = "Our Blacksmith is a point of pride to the people "
"of Tristram.  Not only is he a master craftsman who has won many "
"contests within his guild, but he received praises from our King "
"Leoric himself - may his soul rest in peace.  Griswold is also a great hero; just "
"ask Cain. |";

static char Gillian5[] = "Cain has been the storyteller of Tristram for "
"as long as I can remember.  He knows so much, and can tell you just "
"about anything about almost everything. |";

static char Gillian6[] = "Farnham is a drunkard who fills his belly with "
"ale and everyone else's ears with nonsense. \n \n"
"I know that both Pepin "
"and Ogden feel sympathy for him, but I get so frustrated watching him "
"slip farther and farther into a befuddled stupor every night. |";

static char Gillian7[] = "Pepin saved my grandmother's life, and I know "
"that I can never repay him for that.  His ability to heal any sickness "
"is more powerful than the mightiest sword and more mysterious than any spell you "
"can name.  If you ever are in need of healing, Pepin can help you. |";

static char Gillian9[] = "I grew up with Wirt's mother, Canace.  Although "
"she was only slightly hurt when those hideous creatures stole him, "
"she never recovered.  I think she died of a broken heart.  Wirt has "
"become a mean-spirited youngster, looking only to profit from the "
"sweat of others.  I know that he suffered and has seen horrors that "
"I cannot even imagine, but some of that darkness hangs over him still. |";

static char Gillian10[] = "Ogden and his wife have taken me and my "
"grandmother into their home and have even let me earn a few gold "
"pieces by working at the inn.  I owe so much to them, and hope one "
"day to leave this place and help them start a grand hotel in the east. |";
#endif

//	Griswold
static char Griswold1[] = "Well, what can I do for ya? |";

#if !IS_VERSION(SHAREWARE)
static char Griswold2[] = "If you're looking for a good weapon, let me show "
"this to you.  Take your basic blunt weapon, such as a mace.  Works like "
"a charm against most of those undying horrors down there, and there's "
"nothing better to shatter skinny little skeletons! |";

static char Griswold3[] = "The axe?  Aye, that's a good weapon, balanced against any "
"foe.  Look how it cleaves the air, and then imagine a nice fat demon "
"head in its path.  Keep in mind, however, that it is slow to swing - "
"but talk about dealing a heavy blow! |";

static char Griswold4[] = "Look at that edge, that balance.  A sword in "
"the right hands, and against the right foe, is the master of all "
"weapons.  Its keen blade finds little to hack or pierce on the undead, "
"but against a living, breathing enemy, a sword will better slice their "
"flesh! |";

static char Griswold5[] = "Your weapons and armor will show the signs of "
"your struggles against the Darkness.  If you bring them to me, with a "
"bit of work and a hot forge, I can restore them to top fighting form. |";

static char Griswold6[] = "While I have to practically smuggle in the "
"metals and tools I need from caravans that skirt the edges of our "
"damned town, that witch, Adria, always seems to get whatever she needs.  "
"If I knew even the smallest bit about how to harness magic as she did, "
"I could make some truly incredible things. |";

static char Griswold7[] = "Gillian is a nice lass.  Shame that her gammer "
"is in such poor health or I would arrange to get both of them out of "
"here on one of the trading caravans. |";

static char Griswold8[] = "Sometimes I think that Cain talks too much, "
"but I guess that is his calling in life.  If I could bend steel as "
"well as he can bend your ear, I could make a suit of court plate "
"good enough for an Emperor! |";

static char Griswold9[] = "I was with Farnham that night that Lazarus "
"led us into Labyrinth.  I never saw the Archbishop again, and I may "
"not have survived if Farnham was not at my side.  I fear that the "
"attack left his soul as crippled as, well, another did my leg.  I "
"cannot fight this battle for him now, but I would if I could. |";

static char Griswold10[] = "A good man who puts the needs of others above "
"his own.  You won't find anyone left in Tristram - or anywhere else "
"for that matter - who has a bad thing to say about the healer. |";

static char Griswold12[] = "That lad is going to get himself into serious "
"trouble... or I guess I should say, again.  I've tried to interest him "
"in working here and learning an honest trade, but he prefers the high "
"profits of dealing in goods of dubious origin.  I cannot hold that "
"against him after what happened to him, but I do wish he would at "
"least be careful. |";

static char Griswold13[] = "The Innkeeper has little business and no real "
"way of turning a profit.  He manages to make ends meet by providing "
"food and lodging for those who occasionally drift through the "
"village, but they are as likely to sneak off into the night as they "
"are to pay him.  If it weren't for the stores of grains and dried "
"meats he kept in his cellar, why, most of us would have starved during "
"that first year when the entire countryside was overrun by demons. |";
#endif

//	Farnham
static char Farnham1[] = "Can't a fella drink in peace? |";

#if !IS_VERSION(SHAREWARE)
static char Farnham2[] = "The gal who brings the drinks?  Oh, yeah, what a "
"pretty lady.  So nice, too. |";

static char Farnham3[] = "Why don't that old crone do somethin' for a "
"change.  Sure, sure, she's got stuff, but you listen to me... she's "
"unnatural.  I ain't never seen her eat or drink - and you can't trust "
"somebody who doesn't drink at least a little. |";

static char Farnham4[] = "Cain isn't what he says he is.  Sure, sure, he "
"talks a good story... some of 'em are real scary or funny... but I think "
"he knows more than he knows he knows. |";

static char Farnham5[] = "Griswold?  Good old Griswold.  I love him like "
"a brother!  We fought together, you know, back when... we... Lazarus... "
" Lazarus... Lazarus!!! |";

static char Farnham6[] = "Hehehe, I like Pepin.  He really tries, you know.  "
"Listen here, you should make sure you get to know him. Good fella like "
"that with people always wantin' help.  Hey, I guess that would be kinda "
"like you, huh hero?  I was a hero too... |";

static char Farnham8[] = "Wirt is a kid with more problems than even me, "
"and I know all about problems.  Listen here - that kid is gotta sweet "
"deal, but he's been there, you know?  Lost a leg!  Gotta walk around on "
"a piece of wood.  So sad, so sad... |";

static char Farnham9[] = "Ogden is the best man in town.  I don't think his "
"wife likes me much, but as long as she keeps tappin' kegs, I'll like "
"her just fine.  Seems like I been spendin' more time with Ogden than "
"most, but he's so good to me... |";

static char Farnham10[] = "I wanna tell ya sumthin', 'cause I know all about this "
"stuff.  It's my specialty.  This here is "
"the best... theeeee best!  That other ale ain't no good since "
"those stupid dogs... |";

static char Farnham11[] = "No one ever lis... listens to me.  Somewhere - "
"I ain't too sure - but somewhere under the church is a whole pile o' "
"gold.  Gleamin' and shinin' and just waitin' for someone to get it. |";

static char Farnham12[] = "I know you gots your own ideas, and I know you're "
"not gonna believe this, but that weapon you got there - it just ain't "
"no good against those big brutes!  Oh, I don't care what Griswold "
"says, they can't make anything like they used to in the old days... |";

static char Farnham13[] = "If I was you... and I ain't... but if I was, I'd "
"sell all that stuff you got and get out of here.  That boy out there... "
"He's always got somethin good, but you gotta give him some gold or "
"he won't even show you what he's got. |";
#endif

//	Adria
static char Adria1[] = "I sense a soul in search of answers... |";

#if !IS_VERSION(SHAREWARE)
static char Adria2[] = "Wisdom is earned, not given.  If you discover a "
"tome of knowledge, devour its words.  Should you already have knowledge "
"of the arcane mysteries scribed within a book, remember - that level of "
"mastery can always increase. |";

static char Adria3[] = "The greatest power is often the shortest lived.  "
"You may find ancient words of power written upon scrolls of parchment.  "
"The strength of these scrolls lies in the ability of either apprentice "
"or adept to cast them with equal ability.  Their weakness is that they "
"must first be read aloud and can never be kept at the ready in your "
"mind.  Know also that these scrolls can be read but once, so use them "
"with care. |";

static char Adria4[] = "Though the heat of the sun is beyond measure, the "
"mere flame of a candle is of greater danger.  No energies, no matter how "
"great, can be used without the proper focus.  For many spells, "
"ensorcelled Staves may be charged with magical energies many times "
"over.  I have the ability to restore their power - but know that nothing "
"is done without a price. |";

static char Adria5[] = "The sum of our knowledge is in the sum of its "
"people.  Should you find a book or scroll that you cannot decipher, do "
"not hesitate to bring it to me.  If I can make sense of it I will share "
"what I find. |";

static char Adria6[] = "To a man who only knows Iron, there is no greater "
"magic than Steel.  The blacksmith Griswold is more of a sorcerer than "
"he knows.  His ability to meld fire and metal is unequaled in this land. |";

static char Adria7[] = "Corruption has the strength of deceit, but innocence "
"holds the power of purity.  The young woman Gillian has a pure heart, "
"placing the needs of her matriarch over her own.  She fears me, but it is "
"only because she does not understand me. |";

static char Adria8[] = "A chest opened in darkness holds no greater treasure "
"than when it is opened in the light.  The storyteller Cain is an enigma, "
"but only to those who do not look.  His knowledge of what lies beneath "
"the cathedral is far greater than even he allows himself to realize. |";

static char Adria9[] = "The higher you place your faith in one man, the "
"farther it has to fall.  Farnham has lost his soul, but not to any demon.  "
"It was lost when he saw his fellow townspeople betrayed by the "
"Archbishop Lazarus.  He has knowledge to be gleaned, but you must "
"separate fact from fantasy. |";

static char Adria10[] = "The hand, the heart and the mind can perform "
"miracles when they are in perfect harmony.  The healer Pepin sees into "
"the body in a way that even I cannot.  His ability to restore the sick "
"and injured is magnified by his understanding of the creation of "
"elixirs and potions.  He is as great an ally as you have in Tristram. |";

static char Adria12[] = "There is much about the future we cannot see, but "
"when it comes it will be the children who wield it.  The boy Wirt has "
"a blackness upon his soul, but he poses no threat to the town or its "
"people.  His secretive dealings with the urchins and unspoken guilds "
"of nearby towns gain him access to many devices that cannot be easily "
"found in Tristram.  While his methods may be reproachful, Wirt can "
"provide assistance for your battle against the encroaching Darkness. |";

static char Adria13[] = "Earthen walls and thatched canopy do not a home "
"create.  The innkeeper Ogden serves more of a purpose in this town than "
"many understand.  He provides shelter for Gillian and her matriarch, "
"maintains what life Farnham has left to him, and provides an anchor "
"for all who are left in the town to what Tristram once was.  His tavern, "
"and the simple pleasures that can still be found there, provide a "
"glimpse of a life that the people here remember.  It is that memory "
"that continues to feed their hopes for your success. |";
#endif

//	Wirt
static char Wirt1[] = "Pssst... over here... |";
	
#if !IS_VERSION(SHAREWARE)
static char Wirt2[] = "Not everyone in Tristram has a use - or a market - "
"for everything you will find in the labyrinth.  Not even me, as hard "
"as that is to believe. \n \n"
"Sometimes, only you will be able to find a "
"purpose for some things. |";

static char Wirt3[] = "Don't trust everything the drunk says.  Too many "
"ales have fogged his vision and his good sense. |";

static char Wirt4[] = "In case you haven't noticed, I don't buy anything "
"from Tristram.  I am an importer of quality goods.  If you want to "
"peddle junk, you'll have to see Griswold, Pepin or that witch, "
"Adria.  I'm sure that they will snap up whatever you can bring them... |";

static char Wirt5[] = "I guess I owe the blacksmith my life - what there "
"is of it.  Sure, Griswold offered me an apprenticeship at the smithy, "
"and he is a nice enough guy, but I'll never get enough money to... "
"well, let's just say that I have definite plans that require a large "
"amount of gold. |";

static char Wirt6[] = "If I were a few years older, I would shower her "
"with whatever riches I could muster, and let me assure you I can get "
"my hands on some very nice stuff.  Gillian is a beautiful girl who "
"should get out of Tristram as soon as it is safe.  Hmmm... maybe I'll "
"take her with me when I go... |";

static char Wirt7[] = "Cain knows too much.  He scares the life out of me "
"- even more than that woman across the river.  He keeps telling me "
"about how lucky I am to be alive, and how my story is foretold in "
"legend.  I think he's off his crock. |";

static char Wirt8[] = "Farnham - now there is a man with serious problems, "
"and I know all about how serious problems can be.  He trusted too much "
"in the integrity of one man, and Lazarus led him into the very jaws of "
"death.  Oh, I know what it's like down there, so don't even start "
"telling me about your plans to destroy the evil that dwells in that "
"Labyrinth.  Just watch your legs... |";

static char Wirt9[] = "As long as you don't need anything reattached, "
"old Pepin is as good as they come. \n \n"
"If I'd have had some of those "
"potions he brews, I might still have my leg... |";

static char Wirt11[] = "Adria truly bothers me.  Sure, Cain is creepy in "
"what he can tell you about the past, but that witch can see into "
"your past.  She always has some way to get whatever she needs, too.  "
"Adria gets her hands on more merchandise than I've seen pass through "
"the gates of the King's Bazaar during High Festival. |";

static char Wirt12[] = "Ogden is a fool for staying here.  I could get "
"him out of town for a very reasonable price, but he insists on trying "
"to make a go of it with that stupid tavern.  I guess at the least he "
"gives Gillian a place to work, and his wife Garda does make a superb "
"Shepherd's pie... |";
#endif

/*-----------------------------------------------------------------------**
//	BOOKS and SCROLLS as read by the heroes
**-----------------------------------------------------------------------*/

static char Cow1[] = " |"; 

/*-----------------------------------------------------------------------**
//	BOOKS and SCROLLS as read by the heroes
**-----------------------------------------------------------------------*/
#if !IS_VERSION(SHAREWARE)
//	Warrior, Sorceror, Rogue
//SKELETAL TOME (TRIGGERS THE QUEST): 
static char Boner[] = "Beyond the Hall of Heroes lies the Chamber of "
"Bone.  Eternal death awaits any who would seek to steal the treasures "
"secured within this room.  So speaks the Lord of Terror, and so it is "
"written. |";

//BOOK OF BLOOD: 
static char Bloody[] = "...and so, locked beyond the Gateway of Blood and "
"past the Hall of Fire, Valor awaits for the Hero of Light to awaken... |";

//BOOK OF THE BLIND (TRIGGERS QUEST): 
static char Blinding[] = "I can see what you see not.\n"
"Vision milky then eyes rot.\n"
"When you turn they will be gone,\n"
"Whispering their hidden song.\n"
"Then you see what cannot be,\n"
"Shadows move where light should be.\n"
"Out of darkness, out of mind,\n"
"Cast down into the Halls of the Blind. |\n";

//STEEL TOME (TRIGGERS QUEST): 
static char Bloodwar[] = "The armories of Hell are home to the Warlord of "
"Blood.  In his wake lay the mutilated bodies of thousands.  Angels and "
"man alike have been cut down to fulfill his endless sacrifices to the "
"Dark ones who scream for one thing - blood. |";
#endif

/*-----------------------------------------------------------------------**
** The Librium of the Horadrim (White Books)
**-----------------------------------------------------------------------*/
#if !IS_VERSION(SHAREWARE)
static char Book11[] = "Take heed and bear witness to the truths that lie herein, for they are the "
"last legacy of the Horadrim.  There is a war that rages on even now, beyond the fields that "
"we know - between the utopian kingdoms of the High Heavens and the chaotic pits of the "
"Burning Hells.  This war is known as the Great Conflict, and it has raged and burned "
"longer than any of the stars in the sky.  Neither side ever gains sway for long as the "
"forces of Light and Darkness constantly vie for control over all creation. |";

static char Book12[] = "Take heed and bear witness to the truths that lie herein, for they are the "
"last legacy of the Horadrim.  When the Eternal Conflict between the High Heavens and the "
"Burning Hells falls upon mortal soil, it is called the Sin War.  Angels and Demons walk "
"amongst humanity in disguise, fighting in secret, away from the prying eyes of mortals.  "
"Some daring, powerful mortals have even allied themselves with either side, and helped "
"to dictate the course of the Sin War. |";

static char Book13[] = "Take heed and bear witness to the truths that lie herein, for they are the "
"last legacy of the Horadrim.  Nearly three hundred years ago, it came to be known that the "
"Three Prime Evils of the Burning Hells had mysteriously come to our world.  The Three "
"Brothers ravaged the lands of the east for decades, while humanity was left trembling "
"in their wake.  Our Order - the Horadrim - was founded by a group of secretive magi to "
"hunt down and capture the Three Evils once and for all.\n \n"
"The original Horadrim captured two of the Three within powerful artifacts known as "
"Soulstones and buried them deep beneath the desolate eastern sands.  The third Evil "
"escaped capture and fled to the west with many of the Horadrim in pursuit.  The Third "
"Evil - known as Diablo, the Lord of Terror - was eventually captured, his essence set "
"in a Soulstone and buried within this Labyrinth.\n \n"
"Be warned that the soulstone must be kept from discovery by those not of the faith.  If "
"Diablo were to be released, he would seek a body that is easily controlled as he would "
"be very weak - perhaps that of an old man or a child. |";
#endif

/*-----------------------------------------------------------------------**
** Grimoire of the Burning Hells (Red Books)
**-----------------------------------------------------------------------*/
#if !IS_VERSION(SHAREWARE)
static char Book21[] = "So it came to be that there was a great revolution within the Burning "
"Hells known as The Dark Exile.  The Lesser Evils overthrew the Three Prime Evils and banished "
"their spirit forms to the mortal realm.  The demons Belial (the Lord of Lies) and "
"Azmodan (the Lord of Sin) fought to claim rulership of Hell during the absence of "
"the Three Brothers.  All of Hell polarized between the factions of Belial and Azmodan "
"while the forces of the High Heavens continually battered upon the very Gates of Hell. |";

static char Book22[] = "Many demons traveled to the mortal realm in search of the "
"Three Brothers.  These "
"demons were followed to the mortal plane by Angels who hunted them throughout the vast "
"cities of the East.  The Angels allied themselves with a secretive Order of mortal magi "
"named the Horadrim, who quickly became adept at hunting demons.  They also made many dark "
"enemies in the underworlds. |";

static char Book23[] = "So it came to be that the Three Prime Evils were banished in spirit form "
"to the mortal realm and after sewing chaos across the East for decades, they were "
"hunted down by the cursed Order of the mortal Horadrim.  The Horadrim used artifacts "
"called Soulstones to contain the essence of Mephisto, the Lord of Hatred and his "
"brother Baal, the Lord of Destruction.  The youngest brother - Diablo, the Lord of "
"Terror - escaped to the west.\n \n"
"Eventually the Horadrim captured Diablo within a Soulstone as well, and buried him "
"under an ancient, forgotten Cathedral.  There, the Lord of Terror sleeps and awaits the "
"time of his rebirth.  Know ye that he will seek a body of youth and power to possess - "
"one that is innocent and easily controlled.  He will then arise to free his Brothers "
"and once more fan the flames of the Sin War... |";
#endif

/*-----------------------------------------------------------------------**
** The Journals of Lazarus the Betrayer (Normal Book Color)
**-----------------------------------------------------------------------*/
#if !IS_VERSION(SHAREWARE)
static char Book31[] = "All praises to Diablo - Lord of Terror and Survivor of The Dark Exile.  "
"When he awakened from his long slumber, my Lord and Master spoke to me of secrets "
"that few mortals know.  He told me the kingdoms of the High Heavens and the pits of "
"the Burning Hells engage in an eternal war.  He revealed the powers that have brought "
"this discord to the realms of man.  My lord has named the battle for this world and "
"all who exist here the Sin War. |";

static char Book32[] = "Glory and Approbation to Diablo - Lord of Terror and Leader of the Three.  "
"My Lord spoke to me of his two Brothers, Mephisto and Baal, who were banished to this "
"world long ago.  My Lord wishes to bide his time and harness his awesome power so that "
"he may free his captive brothers from their tombs beneath the sands of the east.  Once "
"my Lord releases his Brothers, the Sin War will once again know the fury of the Three. |";

static char Book33[] = "Hail and Sacrifice to Diablo - Lord of Terror and Destroyer of Souls.  "
"When I awoke my Master from his sleep, he attempted to possess a mortal's form.  Diablo "
"attempted to claim the body of King Leoric, but my Master was too weak from his "
"imprisonment.  My Lord required a simple and innocent anchor to this world, and so "
"found the boy Albrecht to be perfect for the task.  While the good King Leoric was "
"left maddened by Diablo's unsuccessful possession, I kidnapped his son Albrecht "
"and brought him before my Master.  I now await Diablo's call and pray that I will "
"be rewarded when he at last emerges as the Lord of this world. |";
#endif

static char Intro[] = "Thank goodness you've returned!\n"
"Much has changed since you lived here, my friend.  "
"All was peaceful until the dark riders came and destroyed "
"our village.  Many were cut down where they stood, and those who "
"took up arms were slain or dragged away to become slaves - or worse.  "
"The church at the edge of town has been desecrated and is being used "
"for dark rituals.  The screams that echo in the night are inhuman, but "
"some of our townsfolk may yet survive.  Follow the path that lies "
"between my tavern and the blacksmith shop to find the church and "
"save who you can. \n \n"
"Perhaps I can tell you more if we speak again.  Good luck.|";

// JKEQUEST Place text here

static char CryptMap1[] = "Maintain your quest.  Finding a treasure that is lost is not "
"easy.  Finding a treasure that is hidden less so.  I will leave you with this.  Do not "
"let the sands of time confuse your search.|";

static char CryptMap2[] = "A what?!  This is foolishness.  There's no treasure buried "
"here in Tristram.  Let me see that!!  Ah, Look these drawings are inaccurate.  They "
"don't match our town at all.  I'd keep my mind on what lies below the cathedral and not "
"what lies below our topsoil.|";

static char CryptMap3[] = "I really don't have time to discuss some map you are looking "
"for.  I have many sick people that require my help and yours as well.|";

static char CryptMap4[] = "The once proud Iswall is trapped deep beneath the surface of "
"this world.  His honor stripped and his visage altered.  He is trapped in immortal "
"torment.  Charged to conceal the very thing that could free him.|";

static char CryptMap5[] = "I'll bet that Wirt saw you coming and put on an act just so "
"he could laugh at you later when you were running around the town with your nose in the "
"dirt.  I'd ignore it.|";

static char CryptMap6[] = "There was a time when this town was a frequent stop for "
"travelers from far and wide.  Much has changed since then.  But hidden caves and buried "
"treasure are common fantasies of any child.  Wirt seldom indulges in youthful games.  "
"So it may just be his imagination.|";

static char CryptMap7[] = "Listen here.  Come close.  I don't know if you know what I "
"know, but you've have really got something here.  That's a map.|";

static char CryptMap8[] = "My grandmother often tells me stories about the strange forces "
"that inhabit the graveyard outside of the church.  And it may well interest you to hear "
"one of them.  She said that if you were to leave the proper offering in the cemetary, "
"enter the cathedral to pray for the dead, and then return, the offering would be altered "
"in some strange way.  I don't know if this is just the talk of an old sick woman, but "
"anything seems possible these days.|";

static char CryptMap9[] = "Hmmm.  A vast and mysterious treasure you say.  Mmmm.  "
"Maybe I could be interested in picking up a few things from you.  Or better yet, don't "
"you need some rare and expensive supplies to get you through this ordeal?|";


static char Cowsuit1[] = "Moo.|";

static char Cowsuit2[] = "I said, Moo.|";

static char Cowsuit3[] = "Look I'm just a cow, OK?|";

static char Cowsuit4[] = "All right, all right.  I'm not really a cow.  I don't normally go "
"around like this; but, I was sitting at home minding my own business and all of a sudden "
"these bugs & vines & bulbs & stuff started coming out of the floor... it was horrible!  "
"If only I had something normal to wear, it wouldn't be so bad.  Hey!  Could you go back to "
"my place and get my suit for me?  The brown one, not the gray one, that's for evening wear.  "
"I'd do it myself, but I don't want anyone seeing me like this.  Here, take this, you "
"might need it... to kill those things that have overgrown everything.  You can't miss my "
"house, it's just south of the fork in the river... you know... the one with the overgrown "
"vegetable garden.|";

static char Cowsuit4A[] = "All right, I'll cut the bull.  I didn't mean to steer you wrong.  "
"I was sitting at home, feeling moo-dy, when things got really un-stable; a whole stampede "
"of monsters came out of the floor!  I just cowed.  I just happened to be wearing this Jersey "
"when I ran out the door, and now I look udderly ridiculous.  If only I had something normal "
"to wear, it wouldn't be so bad.  Hey!  Can you go back to my place and get my suit for me?  "
"The brown one, not the gray one, that's for evening wear.  I'd do it myself, but I don't "
"want anyone seeing me like this.  Here, take this, you might need it... to kill those things "
"that have overgrown everything.  You can't miss my house, it's just south of the fork in "
"the river... you know... the one with the overgrown vegetable garden.|";

static char Cowsuit5[] = "What are you wasting time for?  Go get my suit!  And hurry!  That Holstein "
"over there keeps winking at me! |";

static char Cowsuit6[] = "Hey, have you got my suit there?  Quick, pass it over!  These ears "
"itch like you wouldn't believe!|";

static char Cowsuit7[] = "No no no no!  This is my GRAY suit!  It's for evening wear!  "
"Formal occasions!  I can't wear THIS.  What are you, some kind of weirdo?  I need the "
"BROWN suit.|";

static char Cowsuit8[] = "Ahh, that's MUCH better.  Whew!  At last, some dignity!  Are my "
"antlers on straight?  Good.  Look, thanks a lot for helping me out.  Here, take this as a "
"gift; and, you know... a little fashion tip... you could use a little... you could use a "
"new... yknowwhatImean?  The whole adventurer motif is just so... retro.  Just a word of "
"advice, eh?  Ciao.|";

static char Cowsuit9[] = "Look.  I'm a cow.  And you, you're monster bait. "
"Get some experience under your belt!  We'll talk...|";

static char Cowsuit10[] = "Me, I'm a self-made cow.  Make something of "
"yourself, and... then we'll talk.|";

static char Cowsuit11[] = "I don't have to explain myself to every tourist "
"that walks by!  Don't you have some monsters to kill?  Maybe we'll talk "
"later.  If you live...|";

static char Cowsuit12[] = "Quit bugging me.  I'm looking for someone really "
"heroic.  And you're not it.  I can't trust you, you're going to get eaten "
"by monsters any day now... I need someone who's an experienced hero.|";

static char Cornerstone1[] = "And in the year of the Golden Light, it was so decreed "
"that a great Cathedral be raised.  The cornerstone of this holy place was to be "
"carved from the translucent stone Antyrael, named for the Angel who shared his power "
"with the Horadrim.  \n \nIn the Year of Drawing Shadows, the ground shook and the Cathedral "
"shattered and fell.  As the building of catacombs and castles began and man stood "
"against the ravages of the Sin War, the ruins were scavenged for their stones.  And "
"so it was that the cornerstone vanished from the eyes of man. \n \nThe stone was of this "
"world -- and of all worlds -- as the Light is both within all things and beyond all "
"things. Light and unity are the products of this holy foundation, a unity of purpose "
"and a unity of possession.|";

/*"And in the year of Golden Light, it was so "
"decreed that a great cathedral should be raised, for the glory of Heaven.  "
"And for the cornerstone was brought from the Holy Land a block of "
"translucent stone.  And the stone was of this world, and of all worlds, as "
"the spirit of Light is both within and beyond us, in all things.  "
"The stone brought all it touched to all the worlds, uniting them in Light.  "
"In the Year of the Drawing Shadows, the ground shook, and the cathedral "
"tumbled and shattered.  And in the time that followed, of the building of "
"catacombs and castles against the ravages of the Sin War, the ruins were "
"taken for their stones.  And so it was that the Cornerstone vanished from "
"the eyes of man, lost, perhaps forever.|";*/


static char Theo1[] = "Waaaah! (sniff) Waaaah! (sniff)|";

static char Theo2[] = "I lost Theo!  I lost my best friend!  We were playing over by the "
"river, and Theo said he wanted to go look at the big green thing.  I said we "
"shouldn't, but we snuck over there, and then suddenly this BUG came out!  We ran away but "
"Theo fell down and the bug GRABBED him and took him away!|";

static char Theo3[] = "Didja find him?  You gotta find Theodore, please!  "
"He's just little.  He can't take care of himself!  Please!|";

static char Theo4[] = "You found him!  You found him!  Thank you!  Oh Theo, "
"did those nasty bugs scare you?  Hey!  Ugh!  There's something stuck to "
"your fur!  Ick!  Come on, Theo, let's go home!  Thanks again, hero person!|";

static char Farmer1[] = "So, you're the hero everyone's been talking "
"about. Perhaps you could help a poor, simple farmer out of a terrible "
"mess? At the edge of my orchard, just south of here, there's a horrible "
"thing swelling out of the ground! I can't get to my crops or my bales "
"of hay, and my poor cows will starve. The witch gave this to me and "
"said that it would blast that thing out of my field. If you could "
"destroy it, I would be forever grateful. I'd do it myself, but someone "
"has to stay here with the cows...|";

/*"Here, you're that hero everyone's talking about!  "
"You're just the type to help a poor farmer out of a terrible fix!  You "
"see, right at the border of my orchard, just south of here, there's a "
"horrible thing swelling out of the ground!  I daren't go near me own "
"land!  So I got an alchemist to make a magic doo-hickey that'll "
"blast that thing.  All you gotta do is take it over and drop it in.  "
"I'd do it myself, but my leg's acting up, my lumbago, old war wounds, "
"you know. |";*/

static char Farmer2[] = "I knew that it couldn't be as simple as that "
"witch made it sound. It's a sad world when you can't even trust your "
"neighbors.|";
 
/*"You ain't done that measly little thing for me "
"yet?  You're young!  Never a thought for a poor common peasant... "
"it's a sad world.|";*/

static char Farmer2a[] = "It must truly be a fearsome task I've set "
"before you. If there was just some way that I could... would a flagon "
"of some nice, fresh milk help?|";

/*"Please, can you get that snarkfarbin' thing outta "
"myfield?  It's blockin' the path to me privy!|";*/

static char Farmer3[] = "Is it gone? Did you send it back to the dark "
"recesses of Hades that spawned it? You what? Oh, don't tell me you lost "
"it! Those things don't come cheap, you know. You've got to find it, "
"and then blast that horror out of our town.|";

/*"You don't have that doo-hickey no more, but I "
"didn't hear any kaboom!  Don't tell me you lost it!  Those things don't "
"come cheap!  You gotta find it, and blast that slimy-lookin' thing outta "
"there!|";*/

static char Farmer4[] = "I heard the explosion from here! Many thanks "
"to you, kind stranger. What with all these things comin' out of the "
"ground, monsters taking over the church, and so forth, these are trying "
"times. I am but a poor farmer, but here -- take this with my great "
"thanks.|";

/*"You blasted the sucker, eh?  Many thanks to you, you're so kind, "
"many thanks.  I don't know, things comin' out of the ground, monsters taking over the church, "
"these are hard times... I'm a poor man, I ain't got much, but... take this, take this with my"
"  great thanks.|";*/

static char Farmer5[] = "Oh, such a trouble I have...maybe...No, I "
"couldn't impose on you, what with all the other troubles. Maybe after "
"you've cleansed the church of some of those creatures you could come "
"back... and spare a little time to help a poor farmer?|";

/*"Oh, such a trouble I have... maybe... Nah, you're just a kid, "
"still wet behind the ears!  I can't ask you to do this.  If only you were a little better "
"prepared.|";*/

static char Farmer6[] = "Oh, I could use your help, but perhaps after "
"you've saved the catacombs from the desecration of those beasts.|";

/*"Off with you!  What I need done would take a real hero... maybe "
"someday you'll qualify.|";*/

static char Farmer7[] = "I need something done, but I couldn't impose "
"on a perfect stranger. Perhaps after you've been here a while I might "
"feel more comfortable asking a favor.|";

/*"I need something done, but how do I know you won't just skip town "
"on me?  Make a name for yourself, kid, and then come find me.|";*/

static char Farmer8[] = "I see in you the potential for greatness.  "
"Perhaps sometime while you are fulfilling your destiny, you could "
"stop by and do a little favor for me?|";

/*"Talk of the town is that you show real promise!  Maybe if you live "
"up to the rumors... someday, I might have a little job for you.|";*/

static char Farmer9[] = "I think you could probably help me, but perhaps "
"after you've gotten a little more powerful. I wouldn't want to injure "
"the village's only chance to destroy the menace in the church!|";

/*"Busy fighting evil, huh?  Well, if you live long enough to know "
"what you're doing, look me up, I got a little job you might be interested in.|";*/

static char Skulljrnl1[] = "Cloudy and cooler today.  Casting the nets of necromancy across "
"the void landed two new subspecies of flying horror; a good day's work.  Must remember to "
"order some more bat guano and black candles from Adria; I'm running a bit low.|";

static char Skulljrnl2[] = "I have tried spells, threats, abjuration and bargaining with "
"this foul creature -- to no avail.  My methods of enslaving lesser demons seem to have no "
"effect on this fearsome beast.|"; 

/*"I have tried spells, threats, abjuration and bargaining with "
"the new creature.  My means which have enslaved many lesser demons have no effect on this "
"fearsome beast; it only leers at me in a most unpleasant fashion. |";*/

static char Skulljrnl3[] = "My home is slowly becoming corrupted by the vileness of this "
"unwanted prisoner.  The crypts are	full of shadows that move just beyond the corners of "
"my vision.  The faint scrabble of claws dances at the edges of my hearing. They are "
"searching, I think, for this journal.|";

/*"The trapped creature's howls of fury keep me from sleep-- it "
"rages and curses the name of the one who sent it to the void, and against myself, for "
"trapping it here.  Its words fill my heart with terror, yet I cannot block out its voice.|";*/

static char Skulljrnl4[] = "In its ranting, the creature has let slip its name -- "
"Na-Krul.  I have attempted to research the name, but the smaller demons have somehow "
"destroyed my library.  Na-Krul... The name fills me with a cold dread.  I prefer to "
"think of it only as The Creature rather than ponder its true name.|";

/*"My home is coming alive, corrupted by the darkness of my "
"unwanted prisoner... The crypts are full of shadows... I see things at the corners of my "
"vision, hear the sounds of claws, and of searching.  Searching, I think, for me.  For "
"this journal.  They must not learn the secret!  The creature must not be released!|";*/

static char Skulljrnl5[] = "The entrapped creature's howls of fury keep me from gaining "
"much needed sleep.  It rages against the one who sent it to the Void, and it "
"calls foul curses upon me for trapping it here.  Its words fill my heart with terror, "
"and yet I cannot block out its voice.|";

/*"In its ranting, the creature has let slip its name.  It calls "
"itself Na-Krul.  The name is familiar to me... I wish I could research it, but the smaller "
"horrors have destroyed my library in their endless searching.  Still, the syllables fill "
"me with a cold dread... I prefer to think of it only as the creature rather than ponder "
"its true name.|";*/

static char Skulljrnl6[] = "My time is quickly running out.  I must record the ways to "
"weaken the demon, and then conceal that text, lest his minions find some way to use "
"my knowledge to free their lord.  I hope that whoever finds this journal will seek the "
"knowledge.|";
 
/*"Time is running out.  With my last entry I must reveal the way "
"to defeat the demon, then conceal that text, lest the demon-creatures find some way to "
"use my knowledge to free their lord.  I have kept my athame, at least; when all is lost "
"I would rather meet my death at the end of a dagger, than at the claws of a demon.|";*/

static char Skulljrnl7[] = "Whoever finds this scroll is charged with stopping the demonic "
"creature that lies within these walls.  My time is over. Even now, its hellish minions "
"claw at the frail door behind which I hide.  \n \nI have hobbled the demon with arcane "
"magic and encased it within great walls, but I fear that will not be enough. "  
"\n \nThe spells found in my three grimoires will provide you protected entrance to his "
"domain, but only if cast in their proper sequence.  The levers at the entryway "
"will remove the barriers and free the demon; touch them not!  Use only these spells to "
"gain entry or his power may be too great for you to defeat.|";

/*"Whoever finds this, you must stop this demon creature.  It is "
"too late for me now; its hellish minions are clawing at this last door behind which I "
"cower.  I have hobbled the creature by powerful spells and encased it behind thick walls, "
"yet it will not be enough.  You will need the power of the spells in my three grimoires, "
"cast only in their proper sequence.  The levers at the entry will remove the barriers and "
"free the demon; touch them not!  Use only the spells!  The spells in the correct sequence "
"will open the magical doorways, and allow you to face the demon and, I hope, defeat it.  "
"May the Heavens have mercy on us all.|";*/

static char Defiler1[] = "Ahh, mammal, welcome.  Such a... tasty planet you have.  My Nest shall spread "
"across this land, and you and your species shall serve as food for our colony.  I'm sure "
"our meeting will be quite... delicious. |";

static char Defiler2[] = "Have you been enjoying yourself, little mammal?  How pathetic. "
"Your little world will be no challenge at all.|";

static char Defiler3[] = "Come closer, morsel...come find me, the Defiler. I have waited "
"thousands of years...do not try my patience any further. |";

static char Defiler4[] = "Ah, I can smell you...you are close! Close! Ssss...the scent "
"of blood and fear...how enticing...|";

static char Defiler6[] = "We have long lain dormant, and the time to "
"awaken has come.  After our long sleep, we are filled with great hunger.  "
"Soon, now, we shall feed...|";

static char Defiler7[] = "These lands shall be defiled, and our brood "
"shall overrun the fields that men call home.  Our tendrils shall envelop "
"this world, and we will feast on the flesh of its denizens.  Man shall "
"become our chattel and sustenance.|";

static char Defiler8[] = "Come closer, morsel!  I smell your terror, "
"and I hunger.|";

static char Trader1[] = "|";

static char Spell1[] = "In Spiritu Sanctum. |";

static char Spell2[] = "Praedictum Otium. |";

static char Spell3[] = "Efficio Obitus Ut Inimicus. |";

#if IS_VERSION(SHAREWARE)
static char szShareware[] = "Nice try... ";
#endif

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/
//	QST() --> quest in retail version ONLY
// sQST() --> quest in retail or shareware version -- most greetings
#if IS_VERSION(SHAREWARE)
	// make regular quest text into "fake" text
	#define QST(a,b,c,d)	{ &szShareware[0], FALSE, FALSE, TSFX_TAVERN36 }
	#define sQST(a,b,c,d)	{ a,b,c,d }
#else
	#define QST(a,b,c,d)	{ a,b,c,d }
	#define sQST(a,b,c,d)	{ a,b,c,d }
#endif

// * NOTE * Quest text scrolling rate 1 = slowest, 5 = normal, 9 = fastest

const TextDataStruct alltext[] = {
	 QST(	&King1[0],		TRUE,		5,		TSFX_STORY1		),	//	0
	 QST(	&King2[0],		TRUE,		5,		TSFX_TAVERN21	),	//	1
	 QST(	&King3[0],		TRUE,		6,		TSFX_TAVERN22	),	//	2
	 QST(	&King4[0],		TRUE,		5,		TSFX_TAVERN23	),	//	3
	 QST(	&King5[0],		TRUE,		5,		TSFX_HEALER1	),	//	4
	 QST(	&King6[0],		TRUE,		6,		TSFX_BMAID1		),	//	5
	 QST(	&King7[0],		TRUE,		5,		TSFX_SMITH1		),	//	6
	 QST(	&King8[0],		TRUE,		5,		TSFX_DRUNK1		),	//	7
	 QST(	&King9[0],		TRUE,		5,		TSFX_WITCH1		),	//	8
	 QST(	&King10[0],		TRUE,		5,		TSFX_PEGBOY1	),	//	9
	 QST(	&King11[0],		FALSE,		5,		USFX_SKING1		),	//	10
	 QST(	&Banner1[0],	TRUE,		5,		TSFX_STORY2		),	//	11
	 QST(	&Banner2[0],	TRUE,		5,		TSFX_TAVERN24	),	//	12
	 QST(	&Banner3[0],	TRUE,		5,		TSFX_TAVERN25	),	//	13
	 QST(	&Banner4[0],	TRUE,		5,		TSFX_HEALER2	),	//	14
	 QST(	&Banner5[0],	TRUE,		6,		TSFX_BMAID2		),	//	15
	 QST(	&Banner6[0],	TRUE,		6,		TSFX_SMITH2		),	//	16
	 QST(	&Banner7[0],	TRUE,		5,		TSFX_DRUNK2		),	//	17
	 QST(	&Banner8[0],	TRUE,		6,		TSFX_WITCH2		),	//	18
	 QST(	&Banner9[0],	TRUE,		6,		TSFX_PEGBOY2	),	//	19
	 QST(	&Banner10[0],	TRUE,		5,		USFX_SNOT1		),	//	20
	 QST(	&Banner11[0],	TRUE,		6,		USFX_SNOT2		),	//	21
	 QST(	&Banner12[0],	TRUE,		6,		USFX_SNOT3		),	//	22
	 QST(	&Vile1[0],		TRUE,		3,		TSFX_STORY36	),	//	23
	 QST(	&Vile2[0],		TRUE,		5,		TSFX_STORY37	),	//	24
	 QST(	&Vile3[0],		TRUE,		5,		TSFX_STORY38	),	//	25
	 QST(	&Vile4[0],		TRUE,		6,		TSFX_TAVERN1	),	//	26
	 QST(	&Vile5[0],		TRUE,		5,		TSFX_STORY38	),	//	27 not in
	 QST(	&Vile6[0],		TRUE,		5,		TSFX_STORY38	),	//	28 not in
	 QST(	&Vile7[0],		TRUE,		5,		TSFX_HEALER3	),	//	29
	 QST(	&Vile8[0],		TRUE,		5,		TSFX_BMAID3		),	//	30
	 QST(	&Vile9[0],		TRUE,		5,		TSFX_SMITH3		),	//	31
	 QST(	&Vile10[0],		TRUE,		5,		TSFX_DRUNK3		),	//	32
	 QST(	&Vile11[0],		TRUE,		5,		TSFX_WITCH3		),	//	33
	 QST(	&Vile12[0],		TRUE,		5,		TSFX_PEGBOY3	),	//	34
	 QST(	&Vile13[0],		FALSE,		5,		USFX_LAZ1		),	//	35
	 QST(	&Vile14[0],		FALSE,		5,		USFX_LAZ1		),	//	36
	 QST(	&Poison1[0],	TRUE,		5,		TSFX_STORY4		),	//	37
	 QST(	&Poison2[0],	TRUE,		6,		TSFX_TAVERN2	),	//	38
	 QST(	&Poison3[0],	TRUE,		5,		TSFX_HEALER20	),	//	39
	 QST(	&Poison4[0],	TRUE,		6,		TSFX_HEALER21	),	//	40
	 QST(	&Poison5[0],	TRUE,		5,		TSFX_HEALER22	),	//	41
	 QST(	&Poison6[0],	TRUE,		6,		TSFX_BMAID4		),	//	42
	 QST(	&Poison7[0],	TRUE,		5,		TSFX_SMITH4		),	//	43
	 QST(	&Poison8[0],	TRUE,		8,		TSFX_DRUNK4		),	//	44
	 QST(	&Poison9[0],	TRUE,		6,		TSFX_WITCH4		),	//	45
	 QST(	&Poison10[0],	TRUE,		6,		TSFX_PEGBOY4	),	//	46
	 QST(	&Bone1[0],		TRUE,		4,		TSFX_STORY7		),	//	47
	 QST(	&Bone2[0],		TRUE,		6,		TSFX_TAVERN5	),	//	48
	 QST(	&Bone3[0],		TRUE,		6,		TSFX_HEALER5	),	//	49
	 QST(	&Bone4[0],		TRUE,		6,		TSFX_BMAID6		),	//	50
	 QST(	&Bone5[0],		TRUE,		6,		TSFX_SMITH7		),	//	51
	 QST(	&Bone6[0],		TRUE,		5,		TSFX_DRUNK7		),	//	52
	 QST(	&Bone7[0],		TRUE,		6,		TSFX_WITCH7		),	//	53
	 QST(	&Bone8[0],		TRUE,		5,		TSFX_PEGBOY7	),	//	54
	 QST(	&Butch1[0],		TRUE,		5,		TSFX_STORY10	),	//	55
	 QST(	&Butch2[0],		TRUE,		6,		TSFX_TAVERN8	),	//	56
	 QST(	&Butch3[0],		TRUE,		5,		TSFX_HEALER8	),	//	57
	 QST(	&Butch4[0],		TRUE,		6,		TSFX_BMAID8		),	//	58
	 QST(	&Butch5[0],		TRUE,		5,		TSFX_SMITH10	),	//	59
	 QST(	&Butch6[0],		TRUE,		5,		TSFX_DRUNK10	),	//	60
	 QST(	&Butch7[0],		TRUE,		5,		TSFX_WITCH10	),	//	61
	 QST(	&Butch8[0],		TRUE,		6,		TSFX_PEGBOY10	),	//	62
	 QST(	&Butch9[0],		TRUE,		5,		TSFX_WOUND		),	//	63
	 QST(	&Butch10[0],	TRUE,		5,		USFX_CLEAVER	),	//	64
	 QST(	&Blind1[0],		TRUE,		5,		TSFX_STORY12	),	//	65
	 QST(	&Blind2[0],		TRUE,		6,		TSFX_TAVERN10	),	//	66
	 QST(	&Blind3[0],		TRUE,		5,		TSFX_HEALER10	),	//	67
	 QST(	&Blind4[0],		TRUE,		6,		TSFX_BMAID10	),	//	68
	 QST(	&Blind5[0],		TRUE,		6,		TSFX_SMITH12	),	//	69
	 QST(	&Blind6[0],		TRUE,		6,		TSFX_DRUNK12	),	//	70
	 QST(	&Blind7[0],		TRUE,		6,		TSFX_WITCH12	),	//	71
	 QST(	&Blind8[0],		TRUE,		5,		TSFX_PEGBOY11	),	//	72
	 QST(	&Veil1[0],		TRUE,		5,		TSFX_STORY13	),	//	73
	 QST(	&Veil2[0],		TRUE,		6,		TSFX_TAVERN11	),	//	74
	 QST(	&Veil3[0],		TRUE,		5,		TSFX_HEALER11	),	//	75
	 QST(	&Veil4[0],		TRUE,		7,		TSFX_BMAID11	),	//	76
	 QST(	&Veil5[0],		TRUE,		5,		TSFX_SMITH13	),	//	77
	 QST(	&Veil6[0],		TRUE,		5,		TSFX_DRUNK13	),	//	78
	 QST(	&Veil7[0],		TRUE,		6,		TSFX_WITCH13	),	//	79
	 QST(	&Veil8[0],		TRUE,		5,		TSFX_PEGBOY12	),	//	80
	 QST(	&Veil9[0],		TRUE,		3,		USFX_LACH1		),	//	81
	 QST(	&Veil10[0],		TRUE,		6,		USFX_LACH2		),	//	82
	 QST(	&Veil11[0],		TRUE,		4,		USFX_LACH3		),	//	83
	 QST(	&Anvil1[0],		TRUE,		4,		TSFX_STORY14	),	//	84
	 QST(	&Anvil2[0],		TRUE,		7,		TSFX_TAVERN12	),	//	85
	 QST(	&Anvil3[0],		TRUE,		6,		TSFX_HEALER12	),	//	86
	 QST(	&Anvil4[0],		TRUE,		5,		TSFX_BMAID12	),	//	87
	 QST(	&Anvil5[0],		TRUE,		5,		TSFX_SMITH21	),	//	88
	 QST(	&Anvil6[0],		TRUE,		5,		TSFX_SMITH22	),	//	89
	 QST(	&Anvil7[0],		TRUE,		5,		TSFX_SMITH23	),	//	90
	 QST(	&Anvil8[0],		TRUE,		6,		TSFX_DRUNK14	),	//	91
	 QST(	&Anvil9[0],		TRUE,		5,		TSFX_WITCH14	),	//	92
	 QST(	&Anvil10[0],	TRUE,		6,		TSFX_PEGBOY13	),	//	93
	 QST(	&Blood1[0],		TRUE,		3,		TSFX_STORY15	),	//	94
	 QST(	&Blood2[0],		TRUE,		6,		TSFX_TAVERN13	),	//	95
	 QST(	&Blood3[0],		TRUE,		6,		TSFX_HEALER13	),	//	96
	 QST(	&Blood4[0],		TRUE,		6,		TSFX_BMAID13	),	//	97
	 QST(	&Blood5[0],		TRUE,		5,		TSFX_SMITH14	),	//	98
	 QST(	&Blood6[0],		TRUE,		7,		TSFX_DRUNK15	),	//	99
	 QST(	&Blood7[0],		TRUE,		6,		TSFX_WITCH15	),	//	100
	 QST(	&Blood8[0],		TRUE,		6,		TSFX_PEGBOY14	),	//	101
	 QST(	&Warlrd1[0],	TRUE,		5,		TSFX_STORY18	),	//	102
	 QST(	&Warlrd2[0],	TRUE,		6,		TSFX_TAVERN16	),	//	103
	 QST(	&Warlrd3[0],	TRUE,		7,		TSFX_HEALER16	),	//	104
	 QST(	&Warlrd4[0],	TRUE,		6,		TSFX_BMAID16	),	//	105
	 QST(	&Warlrd5[0],	TRUE,		6,		TSFX_SMITH17	),	//	106
	 QST(	&Warlrd6[0],	TRUE,		5,		TSFX_DRUNK17	),	//	107
	 QST(	&Warlrd7[0],	TRUE,		5,		TSFX_WITCH18	),	//	108
	 QST(	&Warlrd8[0],	TRUE,		6,		TSFX_PEGBOY17	),	//	109
	 QST(	&Warlrd9[0],	FALSE,		6,		USFX_WARLRD1	),	//	110
	 QST(	&Infra1[0],		TRUE,		5,		TSFX_STORY20	),	//	111
	 QST(	&Infra2[0],		TRUE,		6,		TSFX_TAVERN18	),	//	112
	 QST(	&Infra3[0],		TRUE,		6,		TSFX_HEALER18	),	//	113
	 QST(	&Infra4[0],		TRUE,		6,		TSFX_BMAID18	),	//	114
	 QST(	&Infra5[0],		TRUE,		5,		TSFX_SMITH24	),	//	115
	 QST(	&Infra6[0],		TRUE,		6,		TSFX_SMITH25	),	//	116
	 QST(	&Infra7[0],		TRUE,		5,		TSFX_SMITH26	),	//	117
	 QST(	&Infra8[0],		TRUE,		5,		TSFX_DRUNK19	),	//	118
	 QST(	&Infra9[0],		TRUE,		5,		TSFX_WITCH20	),	//	119
	 QST(	&Infra10[0],	TRUE,		6,		TSFX_PEGBOY18	),	//	120
	 QST(	&Mush1[0],		TRUE,		5,		TSFX_STORY21	),	//	121
	 QST(	&Mush2[0],		TRUE,		5,		TSFX_TAVERN19	),	//	122
	 QST(	&Mush3[0],		TRUE,		5,		TSFX_HEALER26	),	//	123
	 QST(	&Mush4[0],		TRUE,		5,		TSFX_HEALER27	),	//	124
	 QST(	&Mush5[0],		TRUE,		7,		TSFX_BMAID19	),	//	125
	 QST(	&Mush6[0],		TRUE,		5,		TSFX_SMITH19	),	//	126
	 QST(	&Mush7[0],		TRUE,		5,		TSFX_DRUNK20	),	//	127
	 QST(	&Mush8[0],		TRUE,		5,		TSFX_WITCH22	),	//	128
	 QST(	&Mush9[0],		TRUE,		6,		TSFX_WITCH23	),	//	129
	 QST(	&Mush10[0],		TRUE,		5,		TSFX_WITCH24	),	//	130
	 QST(	&Mush11[0],		TRUE,		5,		TSFX_WITCH25	),	//	131
	 QST(	&Mush12[0],		TRUE,		6,		TSFX_WITCH26	),	//	132
	 QST(	&Mush13[0],		TRUE,		6,		TSFX_PEGBOY19	),	//	133
	 QST(	&Doom1[0],		TRUE,		2,		TSFX_STORY22	),	//	134
	 QST(	&Doom2[0],		TRUE,		6,		TSFX_STORY23	),	//	135
	 QST(	&Doom3[0],		TRUE,		5,		TSFX_STORY24	),	//	136
	 QST(	&Doom4[0],		TRUE,		6,		TSFX_TAVERN20	),	//	137
	 QST(	&Doom5[0],		TRUE,		6,		TSFX_HEALER19	),	//	138
	 QST(	&Doom6[0],		TRUE,		6,		TSFX_BMAID20	),	//	139
	 QST(	&Doom7[0],		TRUE,		6,		TSFX_SMITH20	),	//	140
	 QST(	&Doom8[0],		TRUE,		5,		TSFX_DRUNK21	),	//	141
	 QST(	&Doom9[0],		TRUE,		5,		TSFX_WITCH21	),	//	142
	 QST(	&Doom10[0],		TRUE,		5,		TSFX_PEGBOY20	),	//	143
	 QST(	&Garbud1[0],	TRUE,		6,		USFX_GARBUD1	),	//	144
	 QST(	&Garbud2[0],	TRUE,		6,		USFX_GARBUD2	),	//	145
	 QST(	&Garbud3[0],	TRUE,		6,		USFX_GARBUD3	),	//	146
	 QST(	&Garbud4[0],	TRUE,		6,		USFX_GARBUD4	),	//	147
	 QST(	&Zhar1[0],		TRUE,		6,		USFX_ZHAR1		),	//	148
	 QST(	&Zhar2[0],		TRUE,		7,		USFX_ZHAR2		),	//	149
	sQST(	&Story1[0],		FALSE,		5,		TSFX_STORY25	),	//	150
	 QST(	&Story2[0],		TRUE,		6,		TSFX_STORY26	),	//	151
	 QST(	&Story3[0],		TRUE,		5,		TSFX_STORY27	),	//	152
	 QST(	&Story4[0],		TRUE,		5,		TSFX_STORY28	),	//	153
	 QST(	&Story5[0],		TRUE,		5,		TSFX_STORY29	),	//	154
	 QST(	&Story6[0],		TRUE,		5,		TSFX_STORY30	),	//	155
	 QST(	&Story7[0],		TRUE,		5,		TSFX_STORY31	),	//	156
	 QST(	&Story9[0],		TRUE,		5,		TSFX_STORY33	),	//	157
	 QST(	&Story10[0],	TRUE,		5,		TSFX_STORY34	),	//	158
	 QST(	&Story11[0],	TRUE,		5,		TSFX_STORY35	),	//	159
	sQST(	&Ogden1[0],		FALSE,		5,		TSFX_TAVERN36	),	//	160
	 QST(	&Ogden2[0],		TRUE,		5,		TSFX_TAVERN37	),	//	161
	 QST(	&Ogden3[0],		TRUE,		6,		TSFX_TAVERN38	),	//	162
	 QST(	&Ogden4[0],		TRUE,		6,		TSFX_TAVERN39	),	//	163
	 QST(	&Ogden5[0],		TRUE,		6,		TSFX_TAVERN40	),	//	164
	 QST(	&Ogden6[0],		TRUE,		6,		TSFX_TAVERN41	),	//	165
	 QST(	&Ogden8[0],		TRUE,		6,		TSFX_TAVERN43	),	//	166
	 QST(	&Ogden9[0],		TRUE,		6,		TSFX_TAVERN44	),	//	167
	 QST(	&Ogden10[0],	TRUE,		6,		TSFX_TAVERN45	),	//	168
	sQST(	&Pepin1[0],		FALSE,		5,		TSFX_HEALER37	),	//	169
	 QST(	&Pepin2[0],		TRUE,		5,		TSFX_HEALER38	),	//	170
	 QST(	&Pepin3[0],		TRUE,		5,		TSFX_HEALER39	),	//	171
	 QST(	&Pepin4[0],		TRUE,		5,		TSFX_HEALER40	),	//	172
	 QST(	&Pepin5[0],		TRUE,		5,		TSFX_HEALER41	),	//	173
	 QST(	&Pepin6[0],		TRUE,		5,		TSFX_HEALER42	),	//	174
	 QST(	&Pepin7[0],		TRUE,		5,		TSFX_HEALER43	),	//	175
	 QST(	&Pepin9[0],		TRUE,		5,		TSFX_HEALER45	),	//	176
	 QST(	&Pepin10[0],	TRUE,		5,		TSFX_HEALER46	),	//	177
	 QST(	&Pepin11[0],	TRUE,		6,		TSFX_HEALER47	),	//	178
	sQST(	&Gillian1[0],	FALSE,		5,		TSFX_BMAID31	),	//	179
	 QST(	&Gillian2[0],	TRUE,		6,		TSFX_BMAID32	),	//	180
	 QST(	&Gillian3[0],	TRUE,		6,		TSFX_BMAID33	),	//	181
	 QST(	&Gillian4[0],	TRUE,		5,		TSFX_BMAID34	),	//	182
	 QST(	&Gillian5[0],	TRUE,		6,		TSFX_BMAID35	),	//	183
	 QST(	&Gillian6[0],	TRUE,		6,		TSFX_BMAID36	),	//	184
	 QST(	&Gillian7[0],	TRUE,		5,		TSFX_BMAID37	),	//	185
	 QST(	&Gillian9[0],	TRUE,		5,		TSFX_BMAID39	),	//	186
	 QST(	&Gillian10[0],	TRUE,		5,		TSFX_BMAID40	),	//	187
	sQST(	&Griswold1[0],	FALSE,		5,		TSFX_SMITH44	),	//	188
	 QST(	&Griswold2[0],	TRUE,		5,		TSFX_SMITH45	),	//	189
	 QST(	&Griswold3[0],	TRUE,		5,		TSFX_SMITH46	),	//	190
	 QST(	&Griswold4[0],	TRUE,		5,		TSFX_SMITH47	),	//	191
	 QST(	&Griswold5[0],	TRUE,		6,		TSFX_SMITH48	),	//	192
	 QST(	&Griswold6[0],	TRUE,		5,		TSFX_SMITH49	),	//	193
	 QST(	&Griswold7[0],	TRUE,		6,		TSFX_SMITH50	),	//	194
	 QST(	&Griswold8[0],	TRUE,		5,		TSFX_SMITH51	),	//	195
	 QST(	&Griswold9[0],	TRUE,		5,		TSFX_SMITH52	),	//	196
	 QST(	&Griswold10[0],	TRUE,		6,		TSFX_SMITH53	),	//	197
	 QST(	&Griswold12[0],	TRUE,		5,		TSFX_SMITH55	),	//	198
	 QST(	&Griswold13[0],	TRUE,		5,		TSFX_SMITH56	),	//	199
	sQST(	&Farnham1[0],	FALSE,		5,		TSFX_DRUNK27	),	//	200
	 QST(	&Farnham2[0],	TRUE,		6,		TSFX_DRUNK28	),	//	201
	 QST(	&Farnham3[0],	TRUE,		5,		TSFX_DRUNK29	),	//	202
	 QST(	&Farnham4[0],	TRUE,		5,		TSFX_DRUNK30	),	//	203
	 QST(	&Farnham5[0],	TRUE,		5,		TSFX_DRUNK31	),	//	204
	 QST(	&Farnham6[0],	TRUE,		5,		TSFX_DRUNK32	),	//	205
	 QST(	&Farnham8[0],	TRUE,		5,		TSFX_DRUNK34	),	//	206
	 QST(	&Farnham9[0],	TRUE,		5,		TSFX_DRUNK35	),	//	207
	 QST(	&Farnham10[0],	TRUE,		5,		TSFX_DRUNK23	),	//	208
	 QST(	&Farnham11[0],	TRUE,		5,		TSFX_DRUNK24	),	//	209
	 QST(	&Farnham12[0],	TRUE,		5,		TSFX_DRUNK25	),	//	210
	 QST(	&Farnham13[0],	TRUE,		5,		TSFX_DRUNK26	),	//	211
	sQST(	&Adria1[0],		FALSE,		5,		TSFX_WITCH38	),	//	212
	 QST(	&Adria2[0],		TRUE,		5,		TSFX_WITCH39	),	//	213
	 QST(	&Adria3[0],		TRUE,		5,		TSFX_WITCH40	),	//	214
	 QST(	&Adria4[0],		TRUE,		5,		TSFX_WITCH41	),	//	215
	 QST(	&Adria5[0],		TRUE,		5,		TSFX_WITCH42	),	//	216
	 QST(	&Adria6[0],		TRUE,		5,		TSFX_WITCH43	),	//	217
	 QST(	&Adria7[0],		TRUE,		5,		TSFX_WITCH44	),	//	218
	 QST(	&Adria8[0],		TRUE,		5,		TSFX_WITCH45	),	//	219
	 QST(	&Adria9[0],		TRUE,		5,		TSFX_WITCH46	),	//	220
	 QST(	&Adria10[0],	TRUE,		5,		TSFX_WITCH47	),	//	221
	 QST(	&Adria12[0],	TRUE,		4,		TSFX_WITCH49	),	//	222
	 QST(	&Adria13[0],	TRUE,		4,		TSFX_WITCH50	),	//	223
	sQST(	&Wirt1[0],		FALSE,		5,		TSFX_PEGBOY32	),	//	224
	 QST(	&Wirt2[0],		TRUE,		6,		TSFX_PEGBOY33	),	//	225
	 QST(	&Wirt3[0],		TRUE,		6,		TSFX_PEGBOY34	),	//	226
	 QST(	&Wirt4[0],		TRUE,		5,		TSFX_PEGBOY35	),	//	227
	 QST(	&Wirt5[0],		TRUE,		5,		TSFX_PEGBOY36	),	//	228
	 QST(	&Wirt6[0],		TRUE,		5,		TSFX_PEGBOY37	),	//	229
	 QST(	&Wirt7[0],		TRUE,		5,		TSFX_PEGBOY38	),	//	230
	 QST(	&Wirt8[0],		TRUE,		5,		TSFX_PEGBOY39	),	//	231
	 QST(	&Wirt9[0],		TRUE,		6,		TSFX_PEGBOY40	),	//	232
	 QST(	&Wirt11[0],		TRUE,		5,		TSFX_PEGBOY42	),	//	233
	 QST(	&Wirt12[0],		TRUE,		5,		TSFX_PEGBOY43	),	//	234
	 QST(	&Boner[0],		TRUE,		5,		PS_WARR1		),	//	235
	 QST(	&Bloody[0],		TRUE,		6,		PS_WARR10		),	//	236
	 QST(	&Blinding[0],	TRUE,		5,		PS_WARR11		),	//	237
	 QST(	&Bloodwar[0],	TRUE,		5,		PS_WARR12		),	//	238
	 QST(	&Boner[0],		TRUE,		5,		PS_MAGE1		),	//	239
	 QST(	&Bloody[0],		TRUE,		6,		PS_MAGE10		),	//	240
	 QST(	&Blinding[0],	TRUE,		4,		PS_MAGE11		),	//	241
	 QST(	&Bloodwar[0],	TRUE,		5,		PS_MAGE12		),	//	242
	 QST(	&Boner[0],		TRUE,		5,		PS_ROGUE1		),	//	243
	 QST(	&Bloody[0],		TRUE,		5,		PS_ROGUE10		),	//	244
	 QST(	&Blinding[0],	TRUE,		5,		PS_ROGUE11		),	//	245
	 QST(	&Bloodwar[0],	TRUE,		5,		PS_ROGUE12		),	//	246
	sQST(	&Cow1[0],		FALSE,		5,		TSFX_COW1		),	//	247
	sQST(	&Cow1[0],		FALSE,		5,		TSFX_COW2		),	//	248
	 QST(	&Book11[0],		TRUE,		5,		PS_NAR1			),	//	249
	 QST(	&Book12[0],		TRUE,		4,		PS_NAR2			),	//	250
	 QST(	&Book13[0],		TRUE,		3,		PS_NAR3			),	//	251
	 QST(	&Book21[0],		TRUE,		4,		PS_NAR4			),	//	252
	 QST(	&Book22[0],		TRUE,		5,		PS_NAR5			),	//	253
	 QST(	&Book23[0],		TRUE,		3,		PS_NAR6			),	//	254
	 QST(	&Book31[0],		TRUE,		4,		PS_NAR7			),	//	255
	 QST(	&Book32[0],		TRUE,		4,		PS_NAR8			),	//	256
	 QST(	&Book33[0],		TRUE,		3,		PS_NAR9			),	//	257
	sQST(	&Intro[0],		TRUE,		5,		TSFX_TAVERN0	),	//	258
	 QST(	&Boner[0],		TRUE,		5,		PS_MONK1		),	//	259
	 QST(	&Bloody[0],		TRUE,		5,		PS_MONK10		),	//	260
	 QST(	&Blinding[0],	TRUE,		5,		PS_MONK11		),	//	261
	 QST(	&Bloodwar[0],	TRUE,		5,		PS_MONK12		),	//	262
	 QST(	&Boner[0],		TRUE,		5,		PS_BARD1		),	//	263
	 QST(	&Bloody[0],		TRUE,		5,		PS_BARD10		),	//	264
	 QST(	&Blinding[0],	TRUE,		5,		PS_BARD11		),	//	265

// Link Text and Sound here JKEQUEST
	 QST(	&Bloodwar[0],	TRUE,		5,		PS_BARD12		),	//	266
	 QST(	&CryptMap1[0],	TRUE,		5,		TSFX_WITCH19	),	//  267
	 QST(	&CryptMap2[0],	TRUE,		5,		TSFX_SMITH18	),	//  268
	 QST(	&CryptMap3[0],	TRUE,		5,		TSFX_HEALER17	),	//  269
	 QST(	&CryptMap4[0],	TRUE,		5,		TSFX_WITCH9		),	//  270
	 QST(	&CryptMap5[0],	TRUE,		5,		TSFX_TAVERN17	),	//  271
	 QST(	&CryptMap6[0],	TRUE,		5,		TSFX_STORY19	),	//  272
	 QST(	&CryptMap7[0],	TRUE,		5,		TSFX_DRUNK21	),	//  273
	 QST(	&CryptMap8[0],	TRUE,		5,		TSFX_BMAID27	),	//  274
	 QST(	&CryptMap9[0],	TRUE,		5,		TSFX_PEGBOY7	),	//  275
	 QST(	&CryptMap4[0],	TRUE,		5,		TSFX_WITCH9		),	//  276

	 QST(	&Farmer1[0],	TRUE,		3,		HSFX_FARMER1	),	//  277
	 QST(	&Farmer2[0],	TRUE,		5,		HSFX_FARMER2	),	//  278
	 QST(	&Farmer3[0],	TRUE,		5,		HSFX_FARMER3	),	//  279
	 QST(	&Farmer4[0],	TRUE,		5,		HSFX_FARMER4	),	//  280
	 QST(	&Farmer5[0],	TRUE,		5,		HSFX_FARMER5	),	//  281

	 QST(	&Theo1[0],		TRUE,		5,		HSFX_THEO1		),	//  282
	 QST(	&Theo2[0],		TRUE,		5,		HSFX_THEO2		),	//  283
	 QST(	&Theo3[0],		TRUE,		5,		HSFX_THEO3		),	//  284
	 QST(	&Theo4[0],		TRUE,		5,		HSFX_THEO4		),	//  285

	 QST(	&Defiler6[0],	TRUE,		5,		HSFX_DEFILER6	),	//  286
	 QST(	&Defiler2[0],	TRUE,		5,		HSFX_DEFILER2	),	//  287
	 QST(	&Defiler7[0],	TRUE,		5,		HSFX_DEFILER7	),	//  288
	 QST(	&Defiler4[0],	TRUE,		5,		HSFX_DEFILER4	),	//  289
	 QST(	&Cow1[0],		TRUE,		5,		HSFX_DEFILER5	),	//  290

	 QST(	&Cow1[0],		TRUE,		5,		HSFX_NA_KRUL1	),	//  291
	 QST(	&Cow1[0],		TRUE,		5,		HSFX_NA_KRUL2	),	//  292
	 QST(	&Cow1[0],		TRUE,		5,		HSFX_NA_KRUL3	),	//  293
	 QST(	&Cow1[0],		TRUE,		5,		HSFX_NA_KRUL4	),	//  294
	 QST(	&Cow1[0],		TRUE,		5,		HSFX_NA_KRUL5	),	//  295

	 QST(	&Cornerstone1[0],	TRUE,	2,		HSFX_CORNERSTONE1),	//  296

	 QST(	&Cowsuit1[0],	TRUE,		5,		HSFX_COWSUIT1	),	//  297
	 QST(	&Cowsuit2[0],	TRUE,		5,		HSFX_COWSUIT2	),	//  298
	 QST(	&Cowsuit3[0],	TRUE,		5,		HSFX_COWSUIT3	),	//  299
	 QST(	&Cowsuit4[0],	TRUE,		5,		HSFX_COWSUIT4	),	//  300
	 QST(	&Cowsuit5[0],	TRUE,		5,		HSFX_COWSUIT5	),	//  301
	 QST(	&Cowsuit6[0],	TRUE,		5,		HSFX_COWSUIT6	),	//  302
	 QST(	&Cowsuit7[0],	TRUE,		5,		HSFX_COWSUIT7	),	//  303
	 QST(	&Cowsuit8[0],	TRUE,		5,		HSFX_COWSUIT8	),	//  304
	 QST(	&Cowsuit9[0],	TRUE,		5,		HSFX_COWSUIT9	),	//  305

	 QST(	&Trader1[0],	TRUE,		5,		HSFX_TRADER1	),	//  306

	 QST(	&Farmer2a[0],	TRUE,		5,		HSFX_FARMER2A	),	//  307
	 QST(	&Farmer6[0],	TRUE,		5,		HSFX_FARMER6	),	//  308
	 QST(	&Farmer7[0],	TRUE,		5,		HSFX_FARMER7	),	//  309
	 QST(	&Farmer8[0],	TRUE,		5,		HSFX_FARMER8	),	//  310
	 QST(	&Farmer9[0],	TRUE,		5,		HSFX_FARMER9	),	//  311

	 QST(	&Cowsuit10[0],	TRUE,		5,		HSFX_COWSUIT10	),	//  312
	 QST(	&Cowsuit11[0],	TRUE,		5,		HSFX_COWSUIT11	),	//  313
	 QST(	&Cowsuit12[0],	TRUE,		5,		HSFX_COWSUIT12	),	//  314
	 QST(	&Cowsuit4A[0],	TRUE,		5,		HSFX_COWSUIT4A	),	//  315

	 QST(	&Skulljrnl1[0],	TRUE,		5,		HSFX_SKULLJRNL1	),	//	316
	 QST(	&Skulljrnl2[0],	TRUE,		5,		HSFX_SKULLJRNL2	),	//	317
	 QST(	&Skulljrnl3[0],	TRUE,		5,		HSFX_SKULLJRNL3	),	//	318
	 QST(	&Skulljrnl4[0],	TRUE,		5,		HSFX_SKULLJRNL4	),	//	319
	 QST(	&Skulljrnl5[0],	TRUE,		5,		HSFX_SKULLJRNL5	),	//	320
	 QST(	&Skulljrnl6[0],	TRUE,		5,		HSFX_SKULLJRNL6	),	//	321
	 QST(	&Skulljrnl7[0],	TRUE,		2,		HSFX_SKULLJRNL7	),	//	322

	 QST(	&Spell1[0],		TRUE,		5,		PS_WARR54		),	//	323
	 QST(	&Spell2[0],		TRUE,		5,		PS_WARR55		),	//	324
	 QST(	&Spell3[0],		TRUE,		5,		PS_WARR56		),	//	325

	 QST(	&Spell1[0],		TRUE,		5,		PS_MONK54		),	//	323
	 QST(	&Spell2[0],		TRUE,		5,		PS_MONK55		),	//	324
	 QST(	&Spell3[0],		TRUE,		5,		PS_MONK56		),	//	325

	 QST(	&Spell1[0],		TRUE,		5,		PS_MAGE54		),	//	323
	 QST(	&Spell2[0],		TRUE,		5,		PS_MAGE55		),	//	324
	 QST(	&Spell3[0],		TRUE,		5,		PS_MAGE56		),	//	325

	 QST(	&Spell1[0],		TRUE,		5,		PS_ROGUE54		),	//	323
	 QST(	&Spell2[0],		TRUE,		5,		PS_ROGUE55		),	//	324
	 QST(	&Spell3[0],		TRUE,		5,		PS_ROGUE56		),	//	325

	 QST(	&Spell1[0],		TRUE,		5,		PS_BARD54		),	//	323
	 QST(	&Spell2[0],		TRUE,		5,		PS_BARD55		),	//	324
	 QST(	&Spell3[0],		TRUE,		5,		PS_BARD56		),	//	325

};

const DWORD gdwAllTextEntries = sizeof(alltext) / sizeof(alltext[0]);
