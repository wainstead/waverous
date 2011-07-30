@recycle gwb

@create $thing named George W. Bush action figure, gwb, georgew, bush, dubya 

@describe gwb as "A foot tall action figure of George W. Bush with a smirk on its face and a pull string hanging out its back."

@prop bush.phrases {"Make the pie higher! Make the pie higher!", "Is our children learning?", "I think we all agree, the past is over.", "Will the highways of the Internet become more few?", "I am the pitbull on the pantleg of opportunity.", "I know that the human being and the fish can coexist.", "Put food on your family!", "We cannot let terriers and rogue nations hold this nation hostile or hold our allies hostile.", "Natural gas is hemispheric. I like to call it hemispheric in nature because it is a product that we can find in our neighborhoods.", "The great thing about America is everybody should vote.", "Dick Cheney and I do not want this nation to be in a recession. We want anybody who can find work to be able to find work.", "The legislature's job is to write law. It's the executive branch's job to interpret the law.", "They misunderestimated me.", "They want the federal government controlling Social Security like it's some kind of federal program.", "States should have the right to enact reasonable laws and restrictions particularly to end the inhumane practice of ending a life that otherwise could live.", "The fundamental question is, 'Will I be a successful president when it comes to foreign policy?' I will be, but until I'm the president, it's going to be hard for me to verify that I think I'll be more effective.", "The only things that I can tell you is that every case I have reviewed I have been comfortable with the innocence or guilt of the person that I've looked at. I do not believe we've put a guilty... I mean innocent person to death in the state of Texas.", "I'm gonna talk about the ideal world, Chris. I've read- I understand reality. If you're asking me as the president, would I understand reality, I do.", "Actually, I...this may sound a little West Texan to you, but I like it. When I'm talking about...when I'm talking about myself, and when he's talking about myself, all of us are talking about me.", "It's clearly a budget. It's got a lot of numbers in it.", "I was raised in the West. The west of Texas. It's pretty close to California. In more ways than Washington, D.C., is close to California.", "I understand small business growth. I was one.", "The most important job is not to be governor, or first lady in my case.", "This is Preservation Month. I appreciate preservation. It's what you do when you run for president. You gotta preserve.", "When I was coming up, it was a dangerous world, and you knew exactly who they were. It was us vs. them, and it was clear who them was. Today, we are not so sure who the they are, but we know they're there.", "The administration I'll bring is a group of men and women who are focused on what's best for America, honest men and women, decent men and women, women who will see service to our country as a great privilege and who will not stain the house. ", "This is still a dangerous world. It's a world of madmen and uncertainty and potential mental losses.", "If a person doesn't have the capacity that we all want that person to have, I suspect hope is in the far distant future, if at all. ", "But I also made it clear to (Vladimir Putin) that it's important to think beyond the old days of when we had the concept that if we blew each other up, the world would be safe. ", "First, we would not accept a treaty that would not have been ratified, nor a treaty that I thought made sense for the country. ", "I suspect that had my dad not been president, he'd be asking the same questions: How'd your meeting go with so-and-so? How did you feel when you stood up in front of the people for the State of the Union Address-state of the budget address, whatever you call it.", "We're concerned about AIDS inside our White House - make no mistake about it. ", "There's no such thing as legacies. At least, there is a legacy, but I'll never see it. ", "I appreciate that question because I, in the state of Texas, had heard a lot of discussion about a faith-based initiative eroding the important bridge between church and state. ", "Redefining the role of the United States from enablers to keep the peace to enablers to keep the peace from peacekeepers is going to be an assignment. ", "I know there is a lot of ambition in Washington, obviously. But I hope the ambitious realize that they are more likely to succeed with success as opposed to failure. ", "The California crunch really is the result of not enough power-generating plants and then not enough power to power the power of generating plants.", "If this were a dictatorship, it would be a heck of a lot easier - so long as I'm the dictator. ", "If you don't stand for anything, you don't stand for anything! ", "One of the great things about books is sometimes there are some fantastic pictures.", "It is clear our nation is reliant upon big foreign oil. More and more of our imports come from overseas.", "Well, I think if you say you're going to do something and don't do it, that's trustworthiness.", "We cannot let terrorists and rogue nations hold this nation hostile or hold our allies hostile.", "We hold dear what our Declaration of Independence says, that all have got uninalienable rights, endowed by a Creator.", "It would be a mistake for the United States Senate to allow any kind of human cloning to come out of that chamber.", "This foreign policy stuff is a little frustrating.", "And so, in my State of the - my State of the Union - or state - my speech to the nation, whatever you want to call it, speech to the nation - I asked Americans to give 4,000 years - 4,000 hours over the next - the rest of your life - of service to America. That's what I asked - 4,000 hours.", "Sometimes when I sleep at night I think of (Dr. Seuss's) 'Hop on Pop.'", "I understand that the unrest in the Middle East creates unrest throughout the region.", "I couldn't imagine somebody like Osama bin Laden understanding the joy of Hanukkah.", "I am here to make an announcement that this Thursday, ticket counters and airplanes will fly out of Ronald Reagan Airport.", "We are fully committed to working with both sides to bring the level of terror down to an acceptable level for both.", "One of the interesting initiatives we've taken in Washington, D.C., is we've got these vampire-busting devices. A vampire is a - a cell deal you can plug in the wall to charge your cell phone.", "I want to thank the astronauts who are with us, the courageous spacial entrepreneurs who set such a wonderful example for the young of our country."}

@verb gwb:"p*ull" this

@program gwb:pull
phrase = this.phrases[random($)];
player:tell("You ", verb, " the string on the back of the ", this:title());
player:tell(this:title(), " proclaims, \"", phrase, "\"");
player.location:announce(player:title(), " ", verb, "s the string on the back of the ", this:title());
player.location:announce(this:title(), " proclaims, \"", phrase, "\"");
.

@verb bush:title tnt
@program bush:title
return "^@RED^@George ^@WHITE^@W.^@BLUE^@ Bush^@reset^@ doll";
.


@verb bush:"r*andom" this with any

@program gwb:random
for phrase in (this.phrases)
    if ( index(phrase, iobjstr, 1) )
        player.location:announce_all(player:title(), " pulls the string on the back of the ", this:title());
        player.location:announce_all(this:title(), " proclaims, \"", phrase, "\"");
        break;
    endif
endfor
.

@verb bush:"listall" this

@program gwb:listall
for phrase in (this.phrases)
    player:tell(phrase);
endfor
.


drop gwb

