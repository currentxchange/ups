# Ups 
🌎🔺 Localized curation contract for Antelope blockchains

Ups on WAX testnet ⚗️
[testnet.waxblock.io/account/upsupsupsup3](https://testnet.waxblock.io/account/upsupsupsup3)


# What it does 🤘🚀

The Ups contract creates a Reddit-like upvoting experience on WAX blockchain. 


**Tokenized Curation**

Use Ups to run your own web3 reddit, with communities based around NFT collections or any web domain. 

**What is a `subreddit` here?**

To continue the Reddit analogy, the Ups contract uses `domain`s like a subreddit, except a domain can only be a domain name (like youtube.com) or a colleciton name (like cxcmusicnfts).

**What is a post** 

A 'post' on Reddit is like `content` on Ups. Content can either be a specific URL on a domain, like youtube.com/?v=yourvidzz14, or a template in a collection, like 655780 in collection cxcmusicnfts. 


# 🤔 Why use Ups

Ups creates a gamified economy that benefits all participants. 


## Benefits for collection owners 
Let your collectors rank their favorite NFTs. 

Create demand by:
- Holding upvote competetions for rewards
For example, you could say the top-upped NFT collection this week gets an extra reward for all who hold the template that wins. This creates incentives for collectors that hold more of one template to upvote it. 

- Distribute Reward Tokens 
Use this contract to slow-release your project token in a way that's more engaging that a simple airdrop. *note: the Loot contract to reward your users for staking NFTs in your collection. 



## Benefits for Curators + Upvoters
Earn tokens for curating content. 

- Register content + earn big.
Submitting content gives you rewards each time the content is upvoted. 

- Upvote content + earn 1:1
Every upvote you send will earn you 1 reward.


# Benefits for token project owners
Give new utility to your token.

Run a less-fancy DAO
- Have your community write proposals anywhere. As long as it's the same domain (like Medium, etc) they can be ranked by Ups.

**Why this is so awesome** 
Use your token for the vote token, creating demand. 
Use the reward token to create incentive. 

Proposers will get reward tokens for each upvote that they get on their submission, and voters for their upvotes. This creates large rewards to create a good proposal, and small rewards to vote on them.

You can use this system to get opinions on any set of content that's posted to one platform. Go wild. 

# 🧐 How it works

Content providers can register their domains or NFT collections, and users can "up" content by sending tokens with specific memos. Ups are tracked, and rewards can be claimed by self or any other account can pay the CPU for users' claims, sending them their owed tokens. 

Key Concepts: 
- Upvotes (ups) are quantized to an amount of token. 
- Rewards are also quantized to an amount of a token (can be same or different token)
- 1 Up generates 1 reward for the sumbitter, and one for the upvoter, unless they are the same account, in which case just the upvote reward is registered. 

> By default, the upvoter and the content sumbitter **get 1 reward unit each** for each upvote. (owners can turn off either party getting rewards) There isn't a way to know the true author of content on-chain, so we can't reward the creator directly, it's up to them to submit their content first if they want the reward. 




### For Content Providers:

Step 1: Manage your content and rewards configuration with the `setconfig` action to define reward parameters and token settings. 

Step 2: Register your domain or NFT collection using `regdomain` for domains or regnftcol for NFT collections. 

Step 3: Add content URLs with `addurl` action or NFTs using `addnft`. 




### For Upvoters:

To upvote via URL, send tokens with the memo url|<your url>, replacing <your url> with the actual URL of the content.

To upvote an NFT, use the memo nft|<collection>|<templateid>, where <collection> is the name of the NFT collection and <templateid> is the ID of the template within that collection.

To upvote when you know the content ID, send tokens to the contract with the memo formatted as up|<contentid>, where <contentid> is the ID of the content you wish to upvote.

Use the payup action to claim your rewards. This action calculates and distributes rewards based on the number of upvotes you've contributed and the contract's reward configuration.


# ⏳ For the future + past: Time Units

All votes are assigned a time unit (a number of seconds considered one unit of time) and you can determine exactly what time unit every vote belongs to, allowing you to create **top-charts** that reset every time unit or series of time units. 

This system was originally designed to have one upvote token given to users per time unit, but that functionality isn't in this contract. 

This is one of the first implementations of the web4 concept of time units, based on the [Web4 Manifesto](https://github.com/dougbutner/web-4) written by the contract's author. 

# 🌎 For the world 🤩
Content can (optionally) be assigned a location with, with four levels provided. Included: Continent Subregions (M49), Country (ISO 3166 alpha-3), subdivision aka state, and postal code. Contract only verifies continent subregions and countrys, and allows for passing the three-digit code (like USA) as a string from the user's perspective. 

This means you can hold competetions for the best content in North America, Brazil, etc.  

## Saftey First 👷‍♂️ Rewards & Upvotes ⟁

Key Comprehension:
Rewards are inflationary, upvotes are deflationary. 

If you're planning to have the same reward and upvote token, you should set the reward to be 1/2 or less of the Up amount, otherwise the contract would lose tokens. This configuration would allow the contract to accrue 1/2 of the tokens from upvotes a sumbitter sends to content they submitted. 

You can also set a rewards multiplier at any time.

The only configuration you can't change after setting it is the Timeunit length AFTER the first upvote is made, as it's used to add a temporal aspect to the upvote system. 


### ✋ Pausing Rewards
To temporarily halt reward distribution, the contract owner can use the `pauserewards` action, setting `pause` to `true`. This action stops the conversion of upvotes into claimable rewards without affecting the accumulation of upvotes.

### 🔻 Pausing Upvotes
The `pauseups` action allows the contract owner to freeze the upvote functionality by setting `pause` to `true`. This prevents new upvotes from being recorded, useful during maintenance or adjustments to the system.

## 🎁 Reward Mechanism

Rewards tokens must be held on the contract, or it will stop paying until it has tokens to send. 

### 💸 Claiming
Accumulated rewards are claimed through the `payup` action, which calculates the total rewards based on upvotes and transfers the tokens. This action can be called for any user by any user, provided they pay for the CPU and any RAM costs of the transfer. 

The contract itself can also call this action, so the owner could create an oracle that automatically distributes rewards.

### 🛠 Configuration
The reward system is defined by the `setconfig` action, which sets parameters like the reward token contract, amount per upvote, and any multipliers. 

# ⚠️ Contract Status 

Contract is currently deployed on testnet with testing ongoing. We are still improving the contract. No audit has been performed. Please do not use Ups contract unless on testnet until release 1.0.0 is posted, indicating contract is ready. 

Please open any issues you encounter using this contract on testnet. 

Clone this repo instead of the release for the latest version. 

Ups on WAX testnet ⚗️
[testnet.waxblock.io/account/upsupsupsup3](https://testnet.waxblock.io/account/upsupsupsup3)

## 🧑‍⚖️ Liability waiver
This contract and its functionalities are "AS IS" without warranty of any kind. Always carefully check and compile the latest version of the code on your local environment. Understand the code and it's economic implications before implementing. We are not responsible for  Users should exercise caution and perform their due diligence when interacting with smart contracts on the blockchain.