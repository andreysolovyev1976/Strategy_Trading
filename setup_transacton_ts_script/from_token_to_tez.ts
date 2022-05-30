import {args} from "./cli_args";

import { TezosToolkit } from "@taquito/taquito";
import { InMemorySigner } from "@taquito/signer";
import { swap, batchify } from "@quipuswap/sdk";

const tezos = new TezosToolkit("https://mainnet.api.tez.ie");


tezos.setProvider({
    signer: new InMemorySigner(args.pk)
});

const factories = {
    fa1_2Factory: ['KT1FWHLMk5tHbwuSsp31S4Jum4dTVmkXpfJw', 'KT1Lw8hCoaBrHeTeMXbqHPG4sS4K1xn7yKcD'],
    fa2Factory: ['KT1PvEyN1xCFCgorN92QCfYjw3axS6jawCiJ', 'KT1SwH9P1Tx8a58Mm6qBExQFTcy2rwZyZiXS']
};

const fromAsset1 =  {
    contract: args.someAsset,
};
const fromAsset2 =  {
    contract: args.someAsset,
    id: 0,
};
const toAsset = "tez";

// const inputValue = 1_000_000; // in mutez (without decimals)
(async () => {
    try {
        const swapParams = await swap(
            tezos,
            factories,
            args.factoryType === "FA1_2" ? fromAsset1 : fromAsset2,
            toAsset,
            args.inputValue,
            args.slippageTolerance
        );

        const op = await batchify(tezos.wallet.batch([]), swapParams).send();
        console.info(op.opHash);
        console.info(await op.operationResults());
        await op.confirmation();
        console.info("Complete");

    } catch (err) {
        console.error(err);
    }
})();
