import { parse } from 'ts-command-line-args';

interface ICopyFilesArguments {
    pk: string;
    someAsset: string;
    inputValue: number;
    slippageTolerance: number;
    help?: boolean;
}

export const args = parse<ICopyFilesArguments>(
    {
        pk: String,
        someAsset: String,
        inputValue: Number,
        slippageTolerance: Number,
        help: { type: Boolean, optional: true, alias: 'h', description: 'Prints this usage guide' },
    },
    {
        helpArg: 'help',
        headerContentSections: [{ header: 'Tezos Transaction data', content: 'Parameters of the transaction to execute' }],
        footerContentSections: [{ header: 'Tezos trading bot', content: `Copyright: Andrey Solovyev` }],
    },
);
