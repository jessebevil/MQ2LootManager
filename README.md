MQ2LootManager
This simple loot manager will use EQ's built in Advanced Loot Window to decide how to handle loot. 

If it's set as "Always need" or "Always Greed" it will simply give the item to yourself. 
If it's set as "Never" or "No" it will leave the item on the corpse. 

In all other situations, it will await for the user to make a decision or manually handle the item. 

This is most definitely not as feature rich as MQ2AutoLoot, but there is already an interface for handling loot in everquest. So why not use it?
Sure, it won't automatically distribute items to group members based on a predefined number of quest items set by the user. 
But it also won't automatically give yourself any No Drop items because the default action is keep. The default action is Do NOTHING. 
Meaning you can set it to do nothing, which allows you to manually distribute nodrop items, or handle any number of quest items manually. 
Is this ideal? Perhaps not for you, but it works great for me. 


`/lootmanager` command (which can be shortened as allowed by MQ2) to pause [on|off] or if no selection, will toggle. This is the only command.


I did poke Plure's logic with a stick to get me going in the right direction. Thanx to you sir.