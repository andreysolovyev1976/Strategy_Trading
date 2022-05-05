import {args} from "./cli_args";
// console.info(args);

import { TezosToolkit } from "@taquito/taquito";
import { InMemorySigner } from "@taquito/signer";
import { swap, batchify} from "@quipuswap/sdk";

const tezos = new TezosToolkit("https://hangzhounet.smartpy.io");
// const tezos = new TezosToolkit("https://mainnet-node.madfish.solutions/");

// const publicKeyHash = "tz1fVQangAfb9J1hRRMP2bSB6LvASD6KpY8A";
// const publicKey = "edpkvWbk81uh1DEvdWKR4g1bjyTGhdu1mDvznPUFE2zDwNsLXrEb9K";
// tezos.setSignerProvider(new ReadOnlySigner(publicKeyHash, publicKey));

const privateKey =  {
    contract: args.pk,
    id: 0,
};

tezos.setProvider({
    signer: new InMemorySigner(privateKey.contract),
});

const factories = {
    // fa1_2Factory: "KT1HrQWkSFe7ugihjoMWwQ7p8ja9e18LdUFn",
    // fa2Factory: "KT1Dx3SZ6r4h2BZNQM8xri1CtsdNcAoXLGZB",
    fa1_2Factory: "KT1FWHLMk5tHbwuSsp31S4Jum4dTVmkXpfJw",
    fa2Factory: "KT1PvEyN1xCFCgorN92QCfYjw3axS6jawCiJ",
};

(async () => {
    try {

        const fromAsset = "tez";
        const toAsset =  {
                contract: args.someAsset,
                id: 1,
            };

        // const inputValue = 1_000_000; // in mutez (without decimals)
        const inputValue = {
            contract:  args.inputValue,
            id: 2,
        };

        // const slippageTolerance = 0.005; // 0.5%
        const slippageTolerance = {
            contract: args.slippageTolerance,
            id: 3,
        };

        const swapParams = await swap(
            tezos,
            factories,
            fromAsset,
            toAsset,
            inputValue.contract,
            slippageTolerance.contract
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
