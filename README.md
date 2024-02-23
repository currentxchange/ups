# Ups 
🌎🔺 Localized curation contract for Antelope blockchains

# What it does 🤘🚀

The Ups contract creates a Reddit-like upvoting experience on WAX blockchain. 


**Tokenized Curation**

Use Ups to run your own web3 reddit, with communities based around NFT collections or any web domain. 

**What is a `subreddit` here?**

To continue the Reddit analogy, the Ups contract uses `domain`s like a subreddit, except a domain can only be a domain name (like youtube.com) or a colleciton name (like cxcmusicnfts).

**What is a post** 

A 'post' on Reddit is like `content` on Ups. Content can either be a specific URL on a domain, like youtube.com/?v=yourvidzz14, or a template in a collection, like 655780 in collection cxcmusicnfts. 


# Why use Ups

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

# How it works

Content providers can register their domains or NFT collections, and users can "up" content by sending tokens with specific memos. Ups are tracked, and rewards can be claimed by self or any other account can pay the CPU for users' claims, sending them their owed tokens. 

Key Concepts: 
- Upvotes are quantized to an amount of token. 
- Rewards are also quantized to an amount of a token (can be same or different token)

> By default, the upvoter and the content sumbitter **get 1 reward unit each** for each upvote. (you can turn off either getting rewards) There isn't a way to know the true author of content on-chain, so we can't reward the creator directly, it's up to them to submit their content first if they want the reward. 

### For Content Providers:

Step 1: Register your domain or NFT collection using `regdomain` for domains or regnftcol for NFT collections. 
Step 2: Add content URLs with addurl action or NFTs using addnft. Ensure your content is eligible for upvotes. 
Step 3: Manage your content and rewards configuration with the setconfig action to define reward parameters and token settings. 


### For Upvoters:

To upvote a content ID, send tokens to the contract with the memo formatted as up|<contentid>, where <contentid> is the ID of the content you wish to upvote.
To upvote via URL, send tokens with the memo url|<your url>, replacing <your url> with the actual URL of the content.
To upvote an NFT, use the memo nft|<collection>|<templateid>, where <collection> is the name of the NFT collection and <templateid> is the ID of the template within that collection.
Use the payup action to claim your rewards. This action calculates and distributes rewards based on the number of upvotes you've contributed and the contract's reward configuration.



# Contract Status 

Contract is currently compiled but lacking testing. No audit has been performed. Please do not use Ups contract until release 1.0.0 is released. 

## Disclaimer
This contract and its functionalities are provided "AS IS" without warranty of any kind. Users should exercise caution and perform their due diligence when interacting with smart contracts on the blockchain.