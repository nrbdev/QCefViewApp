export {}

declare global {
    interface Window {
        CallBridge: {
            invoke: (name: string, ...args: any[]) => void
            addEventListener: (name: string, listener: (...args: any[]) => void) => void
            removeEventListener: (name: string, listener: (...args: any[]) => void) => void
        },
        cefViewQuery: (query: { request: string; onSuccess: (response: string) => void; onFailure: (error_code: number, error_message: string) => void }) => Promise<void>;
        cefViewQueryCancel: (id: string) => void
    }
}