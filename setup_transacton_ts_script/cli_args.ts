import { parse } from 'ts-command-line-args';

interface ICopyFilesArguments {
    someAsset: string;
    factoryType: string,
    inputValue: number;
    slippageTolerance: number;
    pk: string;
    help?: boolean;
}

export const args = parse<ICopyFilesArguments>(
    {
        someAsset: String,
        factoryType: String,
        inputValue: Number,
        slippageTolerance: Number,
        pk: String,
        help: { type: Boolean, optional: true, alias: 'h', description: 'Prints this usage guide' },
    },
    {
        helpArg: 'help',
        headerContentSections: [{ header: 'Tezos Transaction data', content: 'Parameters of the transaction to execute' }],
        footerContentSections: [{ header: 'Tezos trading bot', content: `Copyright: Andrey Solovyev` }],
    },
);
