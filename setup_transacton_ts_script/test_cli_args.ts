import { parse } from 'ts-command-line-args';

interface ICopyFilesArguments {
    sourcePath: string;
    targetPath: string;
    help?: boolean;
}

export const args = parse<ICopyFilesArguments>(
    {
        sourcePath: String,
        targetPath: String,
        help: { type: Boolean, optional: true, alias: 'h', description: 'Prints this usage guide' },
    },
    {
        helpArg: 'help',
        headerContentSections: [{ header: 'My Example Config', content: 'Thanks for using Our Awesome Library' }],
        footerContentSections: [{ header: 'Footer', content: `Copyright: Big Faceless Corp. inc.` }],
    },
);
