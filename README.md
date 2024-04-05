# Ups 
🌎🔺 Localized curation contract for Antelope blockchains

Ups on WAX testnet ⚗️
[testnet.waxblock.io/account/upsupsupsup4](https://testnet.waxblock.io/account/upsupsupsup4)



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

- Register content + earn BIG.
Submitting content gives you rewards each time the content is upvoted. 

- Upvote content + earn 1:1 
Every upvote you send will earn you 1 reward. The reward token can be different than the upvore token, or the same token with a different amount.


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
- Upvotes (ups) are **quantized** to an amount of token. 
- Rewards are also quantized to an amount of a token (can be same or different token)
- 1 Up generates 1 reward for the submitter, and one for the upvoter, unless they are the same account, in which case just the upvote reward is registered. 

> By default, the upvoter and the content submitter **get 1 reward unit each** for each upvote. (owners can turn off either party getting rewards) There isn't a way to know the true author of content on-chain, so we can't reward the creator directly, it's up to them to submit their content first if they want the reward. 




### For Content Providers:

Step 1: Manage your content and rewards configuration with the `setconfig` action to define reward parameters and token settings. 

Step 2: Register your domain using steps described in following section, or NFT collection by calling the `regnftcol` action directly. 




### For Upvoters:

**Sending Up Tokens:** Users must send Up tokens to the contract with memos formatted based on their intended action:

- **Content ID Upvote:** `up|<contentid>`
- **URL Registration/Upvote:** `url|<your url>` or `upurl|<your url>`
- **NFT Upvote:** `nft|<collection>|<templateid>` or `upnft|<collection>|<templateid>`
- **NFT Registration:** `addnft|<collection>|<templateid>`
  *Note: You must register the NFT collection before you can register a specific templateid. The contract owner may require that you are an authorized account on the collection.* 
- **URL Registration (No Upvote):** `addurl|<your url>` or `addlink|<your url>`

**Guidelines:**

- Ensure Up tokens sent match or exceed the contract's defined Up token amount. Excess will be refunded.
- Incorrect or unrecognized memos will result in the action not being executed, the transaction will fail (you won't lose your Up tokens).


# ⏳ For the future + past: Time Units

All votes are assigned a time unit (a number of seconds considered one unit of time) and you can determine exactly what time unit every vote belongs to, allowing you to create **top-charts** that reset every time unit or series of time units. 

This system was originally designed to have one upvote token given to users per time unit, but that functionality isn't in this contract. 

This is one of the first implementations of the web4 concept of time units, based on the [Web4 Manifesto](https://github.com/dougbutner/web-4) written by the contract's author. 

# 🌎 For the world 🤩
Content can (optionally) be assigned a location with, with four levels provided. Included: Continent Subregions (M49), Country (ISO 3166 alpha-3), subdivision aka state, and postal code. Contract only verifies continent subregions and countrys, and allows for passing the three-digit code (like USA) as a string from the user's perspective. 

This means you can hold competitions for the best content in North America, Brazil, etc.  

## Rewards & Upvotes ⟁

Key Comprehension:
Rewards are inflationary, upvotes are deflationary. 

If you're planning to have the same reward and upvote token, you should set the reward to be 1/2 or less of the Up amount, otherwise the contract would lose tokens. This configuration would allow the contract to accrue 1/2 of the tokens from upvotes a submitter sends to content they submitted. 

**There's a rewards multiplier, change it at any time.** 

The only configuration you can't change after setting it is the Timeunit length AFTER the first upvote is made, as it's used to add a temporal aspect to the upvote system. 



## Safety First: Essential Precautions 👷‍♂️

Engaging with the Ups contract requires careful consideration of its features and inherent risks:

- **Contract Understanding**: Gain a clear understanding of upvoting, rewards, and content registration processes to prevent unintended transactions.

- **Configuration Specifics**: Pay close attention to the setup of upvotes and rewards. Once the contract is in use, certain settings, like the time unit length for upvotes, cannot be changed. This immutability is critical for maintaining the integrity of the upvote timeline.

- **Economic Dynamics**: Deciding between the same or different tokens for upvotes and rewards has far-reaching economic effects. Balance the inflationary aspect of rewards with the deflationary nature of upvotes to ensure a sustainable token economy.

- **Testnet Practice**: Use the testnet for a risk-free exploration of the contract's capabilities, and to test every action for your use case, including tokenomic implications.

- **Pause Mechanisms**: Familiarize yourself with `pauserewards` and `pauseups` functions for emergency stops or maintenance, ensuring operational control.

Approach Ups interactions with informed caution, prioritizing a comprehensive grasp of functionalities and limitations for a secure experience. Keep in mind that this contract may have bugs, and has not been audited. Use at your own risk.




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
The reward system is defined by the `setconfig` action, which sets parameters like the reward token contract, amount per upvote, and any multipliers. If you're using the waxblocks interface, you must type out `true` for the `pay_upsender` and `pay_submitter` options, as setting the value to `1` will not be recognized and `0` will be stored in the table, resulting in nobody getting paid (records won't be stored in the ious table). 

# ⚠️ Contract Status 
Contract is currently deployed on testnet with testing ongoing. We are still improving the contract. No audit has been performed. Please do not use Ups contract unless on testnet until release 1.0.0 is posted, indicating contract is ready. 

Please open any issues you encounter using this contract on testnet. 

Clone this repo instead of the release for the latest version. 

Ups on WAX testnet ⚗️
[testnet.waxblock.io/account/upsupsupsup4](https://testnet.waxblock.io/account/upsupsupsup4)

## 🧑‍⚖️ Liability waiver
This contract and its functionalities are "AS IS" without warranty of any kind. Always carefully check and compile the latest version of the code on your local environment. Understand the code and it's economic implications before implementing. We (any party involved) are not responsible for any losses or damages that may result in use of our tools including this. We do not offer support outside of Github issues and our [discord](https://discord.com/invite/u3kpj7xEWZ). Users should exercise caution and perform all due diligence when deploying interacting with smart contracts on the blockchain.