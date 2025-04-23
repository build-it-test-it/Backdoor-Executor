import UIKit

class SettingsViewController: UIViewController {
    
    // MARK: - Properties
    
    private let jitService: JITService
    private let tableView: UITableView = {
        let tableView = UITableView(frame: .zero, style: .grouped)
        tableView.translatesAutoresizingMaskIntoConstraints = false
        return tableView
    }()
    
    private enum SettingsSection: Int, CaseIterable {
        case server
        case account
        case about
        
        var title: String {
            switch self {
            case .server: return "Server"
            case .account: return "Account"
            case .about: return "About"
            }
        }
    }
    
    private enum ServerRow: Int, CaseIterable {
        case url
        
        var title: String {
            switch self {
            case .url: return "Server URL"
            }
        }
    }
    
    private enum AccountRow: Int, CaseIterable {
        case deviceInfo
        case resetRegistration
        
        var title: String {
            switch self {
            case .deviceInfo: return "Device Information"
            case .resetRegistration: return "Reset Registration"
            }
        }
    }
    
    private enum AboutRow: Int, CaseIterable {
        case version
        case howItWorks
        
        var title: String {
            switch self {
            case .version: return "Version"
            case .howItWorks: return "How It Works"
            }
        }
    }
    
    // MARK: - Initialization
    
    init(jitService: JITService) {
        self.jitService = jitService
        super.init(nibName: nil, bundle: nil)
    }
    
    required init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    // MARK: - Lifecycle Methods
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        setupUI()
        setupTableView()
    }
    
    // MARK: - UI Setup
    
    private func setupUI() {
        view.backgroundColor = .systemBackground
        title = "Settings"
        
        // Add subviews
        view.addSubview(tableView)
        
        // Set up constraints
        NSLayoutConstraint.activate([
            tableView.topAnchor.constraint(equalTo: view.safeAreaLayoutGuide.topAnchor),
            tableView.leadingAnchor.constraint(equalTo: view.leadingAnchor),
            tableView.trailingAnchor.constraint(equalTo: view.trailingAnchor),
            tableView.bottomAnchor.constraint(equalTo: view.safeAreaLayoutGuide.bottomAnchor)
        ])
    }
    
    private func setupTableView() {
        tableView.delegate = self
        tableView.dataSource = self
        tableView.register(UITableViewCell.self, forCellReuseIdentifier: "SettingsCell")
    }
    
    // MARK: - Actions
    
    private func changeServerURL() {
        let currentURL = KeychainManager.shared.getServerURL() ?? ""
        
        showTextInputAlert(
            title: "Server URL",
            message: "Enter the JIT backend server URL",
            placeholder: "https://your-jit-backend-url.onrender.com",
            defaultText: currentURL
        ) { [weak self] url in
            guard let self = self, let url = url, !url.isEmpty else { return }
            
            KeychainManager.shared.saveServerURL(url)
            self.tableView.reloadData()
            
            // Show confirmation
            self.showAlert(title: "URL Updated", message: "Server URL has been updated. You may need to re-register your device.")
        }
    }
    
    private func showDeviceInfo() {
        let device = Device.current()
        
        let message = """
        Device Name: \(device.name)
        Model: \(device.model)
        iOS Version: \(device.osVersion)
        UDID: \(device.udid)
        """
        
        showAlert(title: "Device Information", message: message)
    }
    
    private func resetRegistration() {
        showConfirmation(
            title: "Reset Registration",
            message: "This will clear your device registration. You will need to register again to use JIT enablement. Continue?",
            yesHandler: { [weak self] in
                guard let self = self else { return }
                
                // Clear auth token
                KeychainManager.shared.deleteAuthToken()
                
                self.showAlert(title: "Registration Reset", message: "Your device registration has been reset.") {
                    // Go back to home screen
                    self.navigationController?.popToRootViewController(animated: true)
                }
            }
        )
    }
    
    private func showHowItWorks() {
        let message = """
        JIT Enabler allows you to enable Just-In-Time (JIT) compilation for iOS apps without modifying the apps themselves.
        
        How it works:
        
        1. The app communicates with a secure backend server
        2. It requests JIT enablement for your selected app
        3. The backend provides instructions specific to your iOS version
        4. The app applies these instructions to enable JIT
        5. Memory permissions are toggled between writable and executable states
        
        This allows apps like emulators and JavaScript engines to run at full speed.
        
        All communication is encrypted and secure.
        """
        
        showAlert(title: "How It Works", message: message)
    }
}

// MARK: - UITableViewDelegate, UITableViewDataSource

extension SettingsViewController: UITableViewDelegate, UITableViewDataSource {
    func numberOfSections(in tableView: UITableView) -> Int {
        return SettingsSection.allCases.count
    }
    
    func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        guard let settingsSection = SettingsSection(rawValue: section) else { return 0 }
        
        switch settingsSection {
        case .server:
            return ServerRow.allCases.count
        case .account:
            return AccountRow.allCases.count
        case .about:
            return AboutRow.allCases.count
        }
    }
    
    func tableView(_ tableView: UITableView, titleForHeaderInSection section: Int) -> String? {
        guard let settingsSection = SettingsSection(rawValue: section) else { return nil }
        return settingsSection.title
    }
    
    func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        let cell = tableView.dequeueReusableCell(withIdentifier: "SettingsCell", for: indexPath)
        
        guard let section = SettingsSection(rawValue: indexPath.section) else { return cell }
        
        switch section {
        case .server:
            guard let row = ServerRow(rawValue: indexPath.row) else { return cell }
            
            switch row {
            case .url:
                cell.textLabel?.text = row.title
                cell.detailTextLabel?.text = KeychainManager.shared.getServerURL()
                cell.accessoryType = .disclosureIndicator
            }
            
        case .account:
            guard let row = AccountRow(rawValue: indexPath.row) else { return cell }
            
            switch row {
            case .deviceInfo:
                cell.textLabel?.text = row.title
                cell.accessoryType = .disclosureIndicator
            case .resetRegistration:
                cell.textLabel?.text = row.title
                cell.textLabel?.textColor = .systemRed
            }
            
        case .about:
            guard let row = AboutRow(rawValue: indexPath.row) else { return cell }
            
            switch row {
            case .version:
                cell.textLabel?.text = row.title
                cell.detailTextLabel?.text = "1.0.0"
                cell.selectionStyle = .none
            case .howItWorks:
                cell.textLabel?.text = row.title
                cell.accessoryType = .disclosureIndicator
            }
        }
        
        return cell
    }
    
    func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
        tableView.deselectRow(at: indexPath, animated: true)
        
        guard let section = SettingsSection(rawValue: indexPath.section) else { return }
        
        switch section {
        case .server:
            guard let row = ServerRow(rawValue: indexPath.row) else { return }
            
            switch row {
            case .url:
                changeServerURL()
            }
            
        case .account:
            guard let row = AccountRow(rawValue: indexPath.row) else { return }
            
            switch row {
            case .deviceInfo:
                showDeviceInfo()
            case .resetRegistration:
                resetRegistration()
            }
            
        case .about:
            guard let row = AboutRow(rawValue: indexPath.row) else { return }
            
            switch row {
            case .version:
                // Do nothing, this is just informational
                break
            case .howItWorks:
                showHowItWorks()
            }
        }
    }
}